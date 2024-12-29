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
                dynamicsWorld->addRigidBody(rigidBodyObj->getRigidBody());
            }
        }
        
        processRigidBodyQueues();
        
        for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++) {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];

            // Ottieni la trasformazione
            btTransform transform;
            btRigidBody* body = btRigidBody::upcast(obj); // Cast a btRigidBody
            if (body && body->getMotionState()) {
                body->getMotionState()->getWorldTransform(transform);
            } else {
                transform = obj->getWorldTransform(); // Per oggetti che non sono btRigidBody
            }

            // Estrai posizione e rotazione
            btVector3 position = transform.getOrigin();
            btQuaternion rotation = transform.getRotation();

            // Stampa i dettagli
            std::cout << "Object " << i << " in dynamicsWorld: " << obj << std::endl;
            std::cout << "  Position: (" << position.getX() << ", " << position.getY() << ", " << position.getZ() << ")" << std::endl;
            std::cout << "  Rotation (quaternion): (" << rotation.getX() << ", " << rotation.getY()
                      << ", " << rotation.getZ() << ", " << rotation.getW() << ")" << std::endl;
        }
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
