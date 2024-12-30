#ifndef EARTH_HPP
#define EARTH_HPP

class Earth: public GameObject {
    
public:
    
    Earth(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props)
    : GameObject(id, m, t, wm, ds, pt, props) {}
    
    void update() override {
        worldMatrix = glm::rotate(worldMatrix, DEG_0_2, Y_AXIS);
    }
    
};

#endif
