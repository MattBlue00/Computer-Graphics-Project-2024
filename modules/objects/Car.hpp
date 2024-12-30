#ifndef CAR_HPP
#define CAR_HPP

class Car: public GameObject {
    
public:
    
    Car(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props)
    : GameObject(id, m, t, wm, ds, pt, props) {}
    
    void update() override {
        float adjustedRoll = std::clamp(carWorldData.roll, -0.005f, 0.005f);
        worldMatrix = MakeWorld(carWorldData.position, carWorldData.yaw, carWorldData.pitch, adjustedRoll);
    }
    
};

#endif
