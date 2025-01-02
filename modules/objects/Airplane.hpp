#ifndef AIRPLANE_HPP
#define AIRPLANE_HPP

class Airplane: public GameObject {
    
    // utility airplane variables and constants
    const float AIRPLANE_MOV_PER_FRAME = 0.5f;
    const float AIRPLANE_FIRST_TURN = 768.0f;
    const float AIRPLANE_SECOND_TURN = -740.0f;
    const float AIRPLANE_LANDING = 700.0f;
    const float AIRPLANE_LAND_MOV_PER_FRAME = 0.1f;
    const float AIRPLANE_LAND_Y = 0.0f;
    const float AIRPLANE_BRAKING_PER_FRAME = 0.00135f;
    float brakingFactor = 1.0f;
    float airplaneAngle = 0;
    int airplaneActionsDone = 0;

public:
    
    Airplane(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props)
    : GameObject(id, m, t, wm, ds, pt, props) {}
    
    void update() override {
        // updates airplane's position
        switch(airplaneActionsDone){
                
            case 0:
                if(worldMatrix[3][2] > AIRPLANE_FIRST_TURN){
                    if(airplaneAngle < 90.0f){
                        worldMatrix = glm::rotate(worldMatrix, -DEG_2_5, Y_AXIS);
                        airplaneAngle += 2.5;
                    }
                    if(airplaneAngle >= 90.0f){
                        airplaneActionsDone++;
                    }
                }
                break;
                
            case 1:
                if(worldMatrix[3][0] < AIRPLANE_SECOND_TURN){
                    if(airplaneAngle < 180.0f){
                        worldMatrix = glm::rotate(worldMatrix, -DEG_2_5, Y_AXIS);
                        airplaneAngle += 2.5;
                    }
                    if(airplaneAngle >= 180.0f){
                        airplaneActionsDone++;
                    }
                }
                break;
            
            case 2:
                if(worldMatrix[3][2] < AIRPLANE_LANDING){
                    if(worldMatrix[3][1] > AIRPLANE_LAND_Y){
                        worldMatrix = glm::translate(worldMatrix, glm::vec3(0.0f, -AIRPLANE_LAND_MOV_PER_FRAME, 0.0f));
                    }
                    if(worldMatrix[3][1] <= AIRPLANE_LAND_Y){
                        airplaneActionsDone++;
                    }
                }
                break;
            
            case 3:
                if(brakingFactor > 0.0f){
                    brakingFactor -= AIRPLANE_BRAKING_PER_FRAME;
                }
                else{
                    brakingFactor = 0;
                    airplaneActionsDone++;
                }
                break;
                
            default:
                break;
                
        }
        
        if(airplaneActionsDone < 4){
            worldMatrix = glm::translate(worldMatrix, glm::vec3(0.0f, 0.0f, AIRPLANE_MOV_PER_FRAME
                                                                      * brakingFactor));
        }
    }
    
};

#endif
