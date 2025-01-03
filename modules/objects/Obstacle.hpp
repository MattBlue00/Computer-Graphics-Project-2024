#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

class Obstacle: public GameObject, public StaticRigidBody {
    
public:
    
    Obstacle(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props) :
    GameObject(id, m, t, wm, ds, pt, props),
    StaticRigidBody(m, wm, 0.8f, 0.5f) {}

};

#endif
