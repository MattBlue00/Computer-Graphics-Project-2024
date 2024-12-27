#ifndef AIRSHIP_HPP
#define AIRSHIP_HPP

class Airship: public GameObject {
    
    // utility airship variables and constants
    const float AIRSHIP_MOV_PER_FRAME = 0.04f;
    bool airshipGoingUp = true;
 
public:
    
    Airship(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : GameObject(id, m, t, wm, ds) {}
    
    void update() override {
        // updates airship's transform matrix
        if(airshipGoingUp){
            if(worldMatrix[3][1] < 3.0f){
                worldMatrix = glm::translate(worldMatrix, glm::vec3(0.0f, AIRSHIP_MOV_PER_FRAME, 0.0f));
            }
            else{
                airshipGoingUp = false;
            }
        }
        else{
            if(worldMatrix[3][1] > -3.0f){
                worldMatrix = glm::translate(worldMatrix, glm::vec3(0.0f, -AIRSHIP_MOV_PER_FRAME, 0.0f));
            }
            else{
                airshipGoingUp = true;
            }
        }
    }
    
};

#endif
