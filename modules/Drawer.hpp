#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "Scene.hpp"
#include "WVP.hpp"
#include "Utils.hpp"

// instance IDs declaration
std::string car = "car";
std::string airplane = "airplane";
std::string airship = "airship";
std::string earth = "earth";
std::string moon = "moon";
std::vector<std::string> car = {"car"};
std::vector<std::string> dir_barrier = { "dir_barrier_oval", "dir_barrier_inner", };
std::vector<std::string> coins = {};
std::vector<std::string> spaceShips = { "space_ship_1", "space_ship_2", "space_ship_3" };
std::vector<std::string> world = {
    "track", "barrier", "finish_line_top", "finish_line_floor", "ramps", "towers", "banners",
    "big_stars", "rainbows", "arcade_1", "arcade_2", "dir_barrier_oval", "dir_banners", "rocket",
    "road_1", "road_2", "road_3", "road_4", "road_5", "road_end_1", "road_end_2", "tires_pile_1",
    "tires_pile_2", "tires_pile_3", "tires_pile_4", "tires_pile_5",
    "traffic_lights", "sun", "asteroids", "gamepad", "police_car", "world", "clouds", "satellite",
    "astronaut", "building"
};

// utility airplane variables and constants
const float AIRPLANE_MOV_PER_FRAME = 0.5f;
const float AIRPLANE_FIRST_TURN = 735.0f;
const float AIRPLANE_SECOND_TURN = -740.0f;
const float AIRPLANE_LANDING = 700.0f;
const float AIRPLANE_LAND_MOV_PER_FRAME = 0.1f;
const float AIRPLANE_LAND_Y = 0.0f;
const float AIRPLANE_BRAKING_PER_FRAME = 0.00135f;
float brakingFactor = 1.0f;
float airplaneAngle = 0;
int airplaneActionsDone = 0;

// utility airship variables and constants
const float AIRSHIP_MOV_PER_FRAME = 0.04f;
bool airshipGoingUp = true;

// utility space ship constants
const float SPACE_SHIP_MAX_DIST = 3000.0f;
const float SPACE_SHIP_MOV_PER_FRAME = 1.2f;

void drawCar(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3** deltaP, float* deltaA, float Pitch, float Roll) {
    int i = scene->InstanceIds[car];

    float adjustedRoll = std::clamp(Roll, -0.005f, 0.005f);
    
    ubo->mMat = MakeWorld(Pos, Yaw + deltaA[i], Pitch, adjustedRoll) * baseCar;
    ubo->mvpMat = ViewPrj * ubo->mMat;
    ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

    scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
    scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
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

void drawDirBarrier(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3** deltaP, float* deltaA, float* usePitch) {
    for (std::vector<std::string>::iterator it = dir_barrier.begin(); it != dir_barrier.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];

        // Condizioni per disegnare o rescalare le barriere
        bool drawOval = firstLap && !secondLap && *it == "dir_barrier_oval";
        bool drawInner = !firstLap && secondLap && *it == "dir_barrier_inner";
        bool rescale = (!firstLap && !secondLap) || (firstLap && !secondLap && *it == "dir_barrier_inner") || (!firstLap && secondLap && *it == "dir_barrier_oval");

        if (drawOval || drawInner) {
            // Disegna la barriera specificata in base al giro
            ubo->mMat = scene->I[i].Wm * baseCar;
            ubo->mvpMat = ViewPrj * ubo->mMat;
            ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));
        }
        else if (rescale) {
            // Rescale other barriers to zero
            glm::mat4 scaledWm = glm::scale(scene->I[i].Wm, glm::vec3(0.0f, 0.0f, 0.0f));
            ubo->mMat = scaledWm * baseCar;
            ubo->mvpMat = ViewPrj * ubo->mMat;
            ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));
        }

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawCoins(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = coins.begin(); it != coins.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
       
        if (coinMap.find(*it) != coinMap.end()) {
            // Coin is present in the map, update its transform matrix
            scene->I[i].Wm = glm::rotate(scene->I[i].Wm, DEG_5, Z_AXIS);
        }
        else {
            // Coin is not present in the map, rescale it to zero
            scene->I[i].Wm = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        }
        
        ubo->mMat = scene->I[i].Wm * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawAirplane(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    int i = scene->InstanceIds[airplane];
    
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
            if(scene->I[i].Wm[3][2] < AIRPLANE_LANDING){
                if(scene->I[i].Wm[3][1] > AIRPLANE_LAND_Y){
                    scene->I[i].Wm = glm::translate(scene->I[i].Wm, glm::vec3(0.0f, -AIRPLANE_LAND_MOV_PER_FRAME, 0.0f));
                }
                if(scene->I[i].Wm[3][1] <= AIRPLANE_LAND_Y){
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
        scene->I[i].Wm = glm::translate(scene->I[i].Wm, glm::vec3(0.0f, 0.0f, AIRPLANE_MOV_PER_FRAME
                                                                  * brakingFactor));
    }
    
    ubo->mMat = scene->I[i].Wm * baseCar;
    ubo->mvpMat = ViewPrj * ubo->mMat;
    ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

    scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
    scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
}

void drawAirship(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    int i = scene->InstanceIds[airship];
    
    // updates airship's transform matrix
    if(airshipGoingUp){
        if(scene->I[i].Wm[3][1] < 3.0f){
            scene->I[i].Wm = glm::translate(scene->I[i].Wm, glm::vec3(0.0f, AIRSHIP_MOV_PER_FRAME, 0.0f));
        }
        else{
            airshipGoingUp = false;
        }
    }
    else{
        if(scene->I[i].Wm[3][1] > -3.0f){
            scene->I[i].Wm = glm::translate(scene->I[i].Wm, glm::vec3(0.0f, -AIRSHIP_MOV_PER_FRAME, 0.0f));
        }
        else{
            airshipGoingUp = true;
        }
    }
    
    ubo->mMat = scene->I[i].Wm * baseCar;
    ubo->mvpMat = ViewPrj * ubo->mMat;
    ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

    scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
    scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
}

void drawEarth(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    int i = scene->InstanceIds[earth];
   
    scene->I[i].Wm = glm::rotate(scene->I[i].Wm, DEG_0_2, Y_AXIS);
    
    ubo->mMat = scene->I[i].Wm * baseCar;
    ubo->mvpMat = ViewPrj * ubo->mMat;
    ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

    scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
    scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
}

void drawMoon(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    int i = scene->InstanceIds[moon];
   
    scene->I[i].Wm = glm::rotate(scene->I[i].Wm, -DEG_0_5, Y_AXIS);
    
    ubo->mMat = scene->I[i].Wm * baseCar;
    ubo->mvpMat = ViewPrj * ubo->mMat;
    ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

    scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
    scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
}

void drawSpaceShips(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = spaceShips.begin(); it != spaceShips.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        if(scene->I[i].Wm[3][0] <= -SPACE_SHIP_MAX_DIST){
            scene->I[i].Wm[3][0] = SPACE_SHIP_MAX_DIST;
        }
        else{
            scene->I[i].Wm = glm::translate(scene->I[i].Wm, glm::vec3(SPACE_SHIP_MOV_PER_FRAME, 0.0f, 0.0f));
        }
        
        ubo->mMat = scene->I[i].Wm * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawAll(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch, float bodyPitch, float bodyRoll){
    // draws the car
    drawCar(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, bodyPitch, bodyRoll);
    
    // draws the circuit and its fixed decorations
    drawWorld(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the dir_barrier that block some path
    drawDirBarrier(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);

    // draws the coins
    drawCoins(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the airplane
    drawAirplane(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the airship
    drawAirship(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the Earth
    drawEarth(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the Moon
    drawMoon(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
    
    // draws the space ships
    drawSpaceShips(scene, gubo, ubo, currentImage, Yaw, Pos, baseCar, ViewPrj, deltaP, deltaA, usePitch);
}

void addInstanceToWorld(std::string instance_id){
    world.push_back(instance_id);
}

void addInstanceToCoins(std::string instance_id){
    coins.push_back(instance_id);
}

#endif
