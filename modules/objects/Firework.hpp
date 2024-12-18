#ifndef FIREWORK_HPP
#define FIREWORK_HPP

class Firework: public GameObject {
    
    int fireworkFrame;
    
    // utility firework constant
    const int MAX_FULL_FIREWORK_FRAMES = 40;
   
public:
    
    Firework(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, int startingFrame)
    : GameObject(id, m, t, wm, ds) {
        fireworkFrame = startingFrame;
    }
    
    void update(PositionData positionData) override {
        if(worldMatrix[0][0] >= 1.0f){
            if(fireworkFrame < MAX_FULL_FIREWORK_FRAMES){
                fireworkFrame += 1;
            }
            else{
                worldMatrix = glm::scale(worldMatrix, glm::vec3(0.001f, 0.001f, 0.001f));
                fireworkFrame = 0;
            }
        }
        else{
            worldMatrix = glm::scale(worldMatrix, glm::vec3(1.05f, 1.05f, 1.05f));
        }
    }
    
};

#endif

