#ifndef DIRECTION_BARRIER_HPP
#define DIRECTION_BARRIER_HPP

#include "../modules/engine/main/physics/RigidBody.hpp"

class DirectionBarrier: public GameObject, public StaticRigidBody {
  
public:
    
    DirectionBarrier(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds) :
    GameObject(id, m, t, wm, ds),
    StaticRigidBody(m, wm, 0.8f, 0.5f) {}
    
};

#endif
