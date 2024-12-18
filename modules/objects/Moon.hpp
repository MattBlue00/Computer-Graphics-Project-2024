#ifndef MOON_HPP
#define MOON_HPP

class Moon: public GameObject {
    
public:
    
    Moon(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : GameObject(id, m, t, wm, ds) {}
    
    void update(PositionData positionData) override {
        worldMatrix = glm::rotate(worldMatrix, -DEG_0_2, Y_AXIS);
    }
    
};

#endif
