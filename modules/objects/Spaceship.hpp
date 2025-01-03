#ifndef SPACESHIP_HPP
#define SPACESHIP_HPP

class Spaceship: public GameObject {
    
    // utility space ship constants
    const float SPACE_SHIP_MAX_DIST = 3000.0f;
    const float SPACE_SHIP_MOV_PER_FRAME = 1.2f;
   
public:
    
    Spaceship(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props)
    : GameObject(id, m, t, wm, ds, pt, props) {}
    
    void update() override {
        if(worldMatrix[3][0] <= -SPACE_SHIP_MAX_DIST){
            worldMatrix[3][0] = SPACE_SHIP_MAX_DIST;
        }
        else{
            worldMatrix = glm::translate(worldMatrix, glm::vec3(SPACE_SHIP_MOV_PER_FRAME, 0.0f, 0.0f));
        }
    }
    
};

#endif
