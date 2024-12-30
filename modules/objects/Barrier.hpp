#ifndef BARRIER_HPP
#define BARRIER_HPP

class Barrier: public GameObject, public StaticRigidBody {
    
public:
    
    Barrier(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props) :
    GameObject(id, m, t, wm, ds, pt, props),
    StaticRigidBody(m, wm, 0.8f, 0.5f) {}
    
};

#endif
