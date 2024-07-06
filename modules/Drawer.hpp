#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "Scene.hpp"
#include "WVP.hpp"
#include "Utils.hpp"

// instance IDs declaration
std::vector<std::string> car = {"car"};
std::vector<std::string> coins = {};
std::vector<std::string> airplane = {"airplane"};
std::vector<std::string> world = {
    "track", "barrier", "finish_line_top", "finish_line_floor", "small_ramps", "airship", "tower", "banners",
    "big_star", "rainbow", "arcade_1", "arcade_2"
};

// utility airplane variables and constants
const float AIRPLANE_MOV_PER_FRAME = 0.425f;
const float AIRPLANE_FIRST_TURN = 735.0f;
const float AIRPLANE_SECOND_TURN = -735.0f;
float airplaneAngle = 0;
int airplaneActionsDone = 0;

void drawCar(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = car.begin(); it != car.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        ubo->mMat = MakeWorld(Pos, Yaw + deltaA[i], usePitch[i], 0) * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawWorld(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = world.begin(); it != world.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        ubo->mMat = scene->I[i].Wm * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawCoins(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = coins.begin(); it != coins.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        // updates coin's transform matrix
        scene->I[i].Wm = glm::rotate(scene->I[i].Wm, DEG_5, Z_AXIS);
        
        ubo->mMat = scene->I[i].Wm * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawAirplane(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = airplane.begin(); it != airplane.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        // updates airplane's position
        switch(airplaneActionsDone){
                
            case 0:
                if(scene->I[i].Wm[3][2] > AIRPLANE_FIRST_TURN){
                    if(airplaneAngle < 90.0f){
                        scene->I[i].Wm = glm::rotate(scene->I[i].Wm, -DEG_2_5, Y_AXIS);
                        airplaneAngle += 2.5;
                    }
                    if(airplaneAngle >= 90.0f){
                        airplaneActionsDone++;
                    }
                }
                break;
                
            case 1:
                if(scene->I[i].Wm[3][0] < AIRPLANE_SECOND_TURN){
                    if(airplaneAngle < 180.0f){
                        scene->I[i].Wm = glm::rotate(scene->I[i].Wm, -DEG_2_5, Y_AXIS);
                        airplaneAngle += 2.5;
                    }
                    if(airplaneAngle >= 180.0f){
                        airplaneActionsDone++;
                    }
                }
                break;
            
            case 2:
                break;
                
            default:
                break;
                
        }
        
        scene->I[i].Wm = glm::translate(scene->I[i].Wm, glm::vec3(0.0f, 0.0f, AIRPLANE_MOV_PER_FRAME));
        
        ubo->mMat = scene->I[i].Wm * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawAll(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    // draws the car
    drawCar(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the circuit and its decorations
    drawWorld(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the coins
    drawCoins(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the airplane
    drawAirplane(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
}

void addInstanceToWorld(std::string instance_id){
    world.push_back(instance_id);
}

void addInstanceToCoins(std::string instance_id){
    coins.push_back(instance_id);
}

#endif
