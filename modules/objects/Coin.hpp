#ifndef COIN_HPP
#define COIN_HPP

class Coin: public GameObject {
    
public:
    
    Coin(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : GameObject(id, m, t, wm, ds) {}
    
    void update(PositionData positionData) override {
        if (active) {
            // Coin is present in the map, update its transform matrix
            worldMatrix = glm::rotate(worldMatrix, DEG_5, Z_AXIS);
        }
        else {
            worldMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
    
};

#endif
