#ifndef CAR_HPP
#define CAR_HPP

class Car: public GameObject {
    
public:
    
    Car(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : GameObject(id, m, t, wm, ds) {}
    
    void update() override {
        float adjustedRoll = std::clamp(carWorldData.roll, -0.005f, 0.005f);
        worldMatrix = MakeWorld(carWorldData.position, carWorldData.yaw, carWorldData.pitch, adjustedRoll);
    }
    
};

#endif
