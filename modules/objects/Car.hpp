#ifndef CAR_HPP
#define CAR_HPP

class Car: public GameObject {
    
public:
    
    Car(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : GameObject(id, m, t, wm, ds) {}
    
    void update(PositionData positionData) override {
        float adjustedRoll = std::clamp(positionData.roll, -0.005f, 0.005f);
        worldMatrix = MakeWorld(positionData.position, positionData.yaw, positionData.pitch, adjustedRoll);
    }
    
};

#endif
