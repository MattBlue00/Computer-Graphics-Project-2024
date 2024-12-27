#ifndef PHYSICS_MANAGER_HPP
#define PHYSICS_MANAGER_HPP

#include <btBulletDynamicsCommon.h>
#include "Utils.hpp"
#include "../engine/main/Scene.hpp"
#include "../engine/main/physics/Collider.hpp"
#include "../engine/main/physics/CollisionCallback.hpp"
#include "../engine/main/physics/RigidBody.hpp"
#include "../modules/data/EngineData.hpp"
#include "../modules/data/WorldData.hpp"

class PhysicsManager : public Manager {
protected:
    // Physics global properties
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;

    void checkCollisions(btRaycastVehicle* vehicle) {
        // Lista per accumulare oggetti da processare
        std::vector<Collider*> collidersToProcess = {};

        // Crea un'istanza del callback
        CollisionCallback collisionCallback(collidersToProcess, vehicle->getRigidBody());

        // Controlla le collisioni con il veicolo
        dynamicsWorld->contactTest(vehicle->getRigidBody(), collisionCallback);

        // Processa le collisioni
        for (Collider* collider : collidersToProcess) {
            collider->onCollision(nullptr);
        }
    }
    
    void processRigidBodyQueues(){
        for (btRigidBody* rigidBody : addRigidBodyQueue){
            dynamicsWorld->addRigidBody(rigidBody);
        }
        addRigidBodyQueue.clear();
        
        for (btRigidBody* rigidBody : removeRigidBodyQueue){
            dynamicsWorld->removeRigidBody(rigidBody);
        }
        removeRigidBodyQueue.clear();
    }

public:
    
    void init() override {
        
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver();
        
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -12.75, 0));
        dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;
        dynamicsWorld->getSolverInfo().m_splitImpulse = true;
        dynamicsWorld->getSolverInfo().m_numIterations = 10;

        
        for (GameObject* obj : gameObjects) {
            RigidBody* rigidBodyObj = dynamic_cast<RigidBody*>(obj);
            if (rigidBodyObj) {
                std::cout << "Adding " << obj->getId() << " to the physics world!\n";
                dynamicsWorld->addRigidBody(rigidBodyObj->getRigidBody());
            }
        }
        
        processRigidBodyQueues();
    }
    
    void update() override {
        dynamicsWorld->stepSimulation(EngineDeltaTime, 60);
        checkCollisions(vehicle);
        processRigidBodyQueues();
    }
    
    void cleanup() override {
        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState()) {
                delete body->getMotionState();
            }
            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }
        
        for (GameObject* obj : gameObjects) {
            RigidBody* rigidBodyObj = dynamic_cast<RigidBody*>(obj);
            if (rigidBodyObj) {
                std::cout << "Deleting the collision shape of " << obj->getId() << "\n";
                delete rigidBodyObj->getCollisionShape();
            }
        }
        
        delete dynamicsWorld;
        delete solver;
        delete dispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }

    btDynamicsWorld* getDynamicsWorld(){
        return dynamicsWorld;
    }
    
};

#endif
