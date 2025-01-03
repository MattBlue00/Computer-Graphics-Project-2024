#ifndef COIN_HPP
#define COIN_HPP

#include "../modules/engine/main/physics/Collider.hpp"
#include "../modules/engine/main/physics/RigidBody.hpp"

#include "../modules/data/WorldData.hpp"

class Coin: public GameObject, public KinematicRigidBody, public Collider {
    
public:
    
    Coin(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props) :
    GameObject(id, m, t, wm, ds, pt, props),
    KinematicRigidBody(new btSphereShape(1.0f), wm),
    Collider() {}
    
    void init() override {
        Collider::init(rigidBody);
    }
    
    void update() override {
        if (enabled) {
            // Coin is present in the world, update its transform matrix
            worldMatrix = glm::rotate(worldMatrix, DEG_5, Z_AXIS);
        }
    }
    
    void onCollision(Collider* other) override {
        if(enabled){
            std::cout << "Collecting " << GameObject::id << std::endl;
            coinCollectedSignal.emit({});
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->removeCollisionObject(this);
            disable();
        }
    }
    
};

#endif
