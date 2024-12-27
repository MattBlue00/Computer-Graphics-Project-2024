#ifndef RAMPS_HPP
#define RAMPS_HPP

class Ramps: public GameObject, public StaticRigidBody {
    
public:
    
    Ramps(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds) :
    GameObject(id, m, t, wm, ds),
    StaticRigidBody(m, wm, 0.0f, 0.0f) {}
    
};

#endif
