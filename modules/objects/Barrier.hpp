#ifndef BARRIER_HPP
#define BARRIER_HPP

class Barrier: public GameObject, public StaticRigidBody {
    
public:
    
    Barrier(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds) :
    GameObject(id, m, t, wm, ds),
    StaticRigidBody(m, wm, 0.8f, 0.5f) {}
    
};

#endif
