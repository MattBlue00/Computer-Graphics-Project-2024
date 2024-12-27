#ifndef COIN_HPP
#define COIN_HPP

#include "../modules/engine/main/physics/Collider.hpp"
#include "../modules/engine/main/physics/RigidBody.hpp"

#include "../modules/data/WorldData.hpp"

class Coin: public GameObject, public KinematicRigidBody, public Collider {
    
public:
    
    Coin(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds) :
    GameObject(id, m, t, wm, ds),
    KinematicRigidBody(new btSphereShape(1.0f), wm),
    Collider() {}
    
    void update() override {
        if (enabled) {
            // Coin is present in the world, update its transform matrix
            worldMatrix = glm::rotate(worldMatrix, DEG_5, Z_AXIS);
        }
    }
    
    void onCollision(Collider* other) override {
        std::cout << "Colliding with " << id << std::endl;
        if(enabled){
            std::cout << "Collecting " << id << std::endl;
            collectedCoins++;
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->removeCollisionObject(this);
            disable();
            std::cout << collectedCoins << std::endl;
        }
    }
    
};

#endif
