#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP

#include "../modules/engine/main/physics/Collider.hpp"
#include "../modules/data/WorldData.hpp"

class Checkpoint: public Collider {

protected:
    
    std::string id;
    btVector3 position;
    btVector3 halfExtents;

    bool hit;
    
    void init(){
        btCollisionShape* checkpointShape = new btBoxShape(halfExtents);
            
        // Creazione della trasformazione per il checkpoint
        btTransform checkpointTransform;
        checkpointTransform.setIdentity();
        checkpointTransform.setOrigin(position);
        
        // Creazione del motion state
        btDefaultMotionState* motionState = new btDefaultMotionState(checkpointTransform);
            
        // Creazione del rigid body senza massa (per oggetto cinematico)
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, checkpointShape, btVector3(0, 0, 0));
        btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
        
        // Imposta il corpo rigido come cinematico
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        
        // Imposta l'ID del checkpoint come user pointer per identificazione
        rigidBody->setUserPointer(this);
        
        addRigidBodyQueue.push_back(rigidBody);
    }
    
public:
    
    Checkpoint(std::string id, btVector3 position, btVector3 halfExtents) : Collider() {
        this->id = id;
        this->position = position;
        this->halfExtents = halfExtents;
        hit = false;
        init();
    }
    
    std::string getId() const { return id; }
    
    btVector3 getPosition() const { return position; }
    
    bool wasHit() const { return hit; }
    
    void reset() {
        hit = false;
    }
    
    void onCollision(Collider* other) override {
        if(!hit && id == nextCheckpointId){
            std::cout << "Hitting " << id << std::endl;
            hit = true;
            updateNextCheckpointSignal.emit({});
        }
    }
    
};

#endif
