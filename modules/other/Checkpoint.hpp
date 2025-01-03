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
    bool enabled;
    
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
        
        Collider::init(rigidBody);
        
        addRigidBodyQueue.push_back(rigidBody);
    }
    
public:
    
    Checkpoint(std::string id, btVector3 position, btVector3 halfExtents) : Collider() {
        this->id = id;
        this->position = position;
        this->halfExtents = halfExtents;
        hit = false;
        enabled = true;
        init();
    }
    
    std::string getId() const { return id; }
    
    btVector3 getPosition() const { return position; }
    
    bool wasHit() const { return hit; }
    
    void reset() {
        hit = false;
    }
    
    void disable() {
        enabled = false;
    }
    
    void onCollision(Collider* other) override {
        if(enabled){
            if(!hit && id == nextCheckpointId){
                std::cout << "Hitting " << id << std::endl;
                hit = true;
                updateNextCheckpointSignal.emit({});
            }
        }
    }
    
};

#endif
