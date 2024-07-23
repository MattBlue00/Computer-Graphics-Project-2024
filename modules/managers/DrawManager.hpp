#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "engine/custom/SceneLoader.hpp"
#include "managers/PhysicsManager.hpp"
#include "tools/WVP.hpp"
#include "tools/Types.hpp"
#include "Utils.hpp"

// instance IDs declaration
std::string car = "car";
std::string airplane = "airplane";
std::string airship = "airship";
std::string earth = "earth";
std::string moon = "moon";
std::vector<std::string> dir_barrier = { "dir_barrier_oval", "dir_barrier_inner" };
std::vector<std::string> coins = {};
std::vector<std::string> spaceShips = { "space_ship_1", "space_ship_2", "space_ship_3" };
std::vector<std::string> fireworks = {
    "firework_1", "firework_2", "firework_3", "firework_4", "firework_5" };
std::vector<std::string> world = {
    "track", "barrier", "finish_line_top", "finish_line_floor", "ramps", "towers", "banners",
    "big_stars", "rainbows", "arcade_1", "arcade_2", "dir_banners", "rocket",
    "road_1", "road_2", "road_3", "road_4", "road_5", "road_end_1", "road_end_2", "tires_pile_1",
    "tires_pile_2", "tires_pile_3", "tires_pile_4", "tires_pile_5", "tires_pile_6", "tires_pile_7",
    "tires_pile_8", "tires_pile_9", "tires_pile_10",
    "traffic_lights", "sun", "asteroids", "gamepad", "police_car", "world", "clouds", "satellite",
    "astronaut", "building"
};

struct DrawManager : public Manager {
    
protected:
    
    Scene* scene;
    UniformBufferObject ubo{};
    GlobalUniformBufferObject gubo{};

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

    // utility fireworks constants and variables
    const int MAX_FULL_FIREWORK_FRAMES = 40;
    std::vector<int> fullFireworkFrames = { 0, 50, 15, 0, 30 };
    
    void drawCar(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj, float Pitch, float Roll) {
        int i = scene->InstanceIds[car];

        float adjustedRoll = std::clamp(Roll, -0.005f, 0.005f);
        
        updateUBO(MakeWorld(Pos, Yaw, Pitch, adjustedRoll), ViewPrj);

        mapToDescriptorSet(scene->DS[i], currentImage);
    }

    void drawWorld(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
        for (std::vector<std::string>::iterator it = world.begin(); it != world.end(); it++) {
            int i = scene->InstanceIds[it->c_str()];
            
            updateUBO(scene->I[i].Wm, ViewPrj);

            mapToDescriptorSet(scene->DS[i], currentImage);
        }
    }

    void drawDirBarrier(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj) {
        for (std::vector<std::string>::iterator it = dir_barrier.begin(); it != dir_barrier.end(); it++) {
            int i = scene->InstanceIds[it->c_str()];

            // Condizioni per disegnare o rescalare le barriere
            bool drawOval = firstLap && !secondLap && *it == "dir_barrier_oval";
            bool drawInner = !firstLap && secondLap && *it == "dir_barrier_inner";
            bool rescale = (!firstLap && !secondLap) || (firstLap && !secondLap && *it == "dir_barrier_inner") || (!firstLap && secondLap && *it == "dir_barrier_oval");

            if (drawOval || drawInner) {
                // Disegna la barriera specificata in base al giro
                updateUBO(scene->I[i].Wm, ViewPrj);
            }
            else if (rescale) {
                // Rescale other barriers to zero
                glm::mat4 scaledWm = glm::scale(scene->I[i].Wm, glm::vec3(0.0f, 0.0f, 0.0f));
                updateUBO(scaledWm, ViewPrj);
            }

            mapToDescriptorSet(scene->DS[i], currentImage);
        }
    }

    void drawCoins(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
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
            
            updateUBO(scene->I[i].Wm, ViewPrj);

            mapToDescriptorSet(scene->DS[i], currentImage);
        }
    }

    void drawAirplane(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
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
        
        updateUBO(scene->I[i].Wm, ViewPrj);

        mapToDescriptorSet(scene->DS[i], currentImage);
    }

    void drawAirship(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
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
        
        updateUBO(scene->I[i].Wm, ViewPrj);

        mapToDescriptorSet(scene->DS[i], currentImage);
    }

    void drawEarth(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
        int i = scene->InstanceIds[earth];
       
        scene->I[i].Wm = glm::rotate(scene->I[i].Wm, DEG_0_2, Y_AXIS);
        
        updateUBO(scene->I[i].Wm, ViewPrj);

        mapToDescriptorSet(scene->DS[i], currentImage);
    }

    void drawMoon(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
        int i = scene->InstanceIds[moon];
       
        scene->I[i].Wm = glm::rotate(scene->I[i].Wm, -DEG_0_2, Y_AXIS);
        
        updateUBO(scene->I[i].Wm, ViewPrj);

        mapToDescriptorSet(scene->DS[i], currentImage);
    }

    void drawSpaceShips(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
        for (std::vector<std::string>::iterator it = spaceShips.begin(); it != spaceShips.end(); it++) {
            int i = scene->InstanceIds[it->c_str()];
            
            if(scene->I[i].Wm[3][0] <= -SPACE_SHIP_MAX_DIST){
                scene->I[i].Wm[3][0] = SPACE_SHIP_MAX_DIST;
            }
            else{
                scene->I[i].Wm = glm::translate(scene->I[i].Wm, glm::vec3(SPACE_SHIP_MOV_PER_FRAME, 0.0f, 0.0f));
            }
            
            updateUBO(scene->I[i].Wm, ViewPrj);

            mapToDescriptorSet(scene->DS[i], currentImage);
        }
    }

    void drawFireworks(int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 ViewPrj){
        int fireworkIndex = 0;
        for (std::vector<std::string>::iterator it = fireworks.begin(); it != fireworks.end(); it++) {
            int i = scene->InstanceIds[it->c_str()];
            
            if(scene->I[i].Wm[0][0] >= 1.0f){
                if(fullFireworkFrames[fireworkIndex] < MAX_FULL_FIREWORK_FRAMES){
                    fullFireworkFrames[fireworkIndex] += 1;
                }
                else{
                    scene->I[i].Wm = glm::scale(scene->I[i].Wm, glm::vec3(0.001f, 0.001f, 0.001f));
                    fullFireworkFrames[fireworkIndex] = 0;
                }
            }
            else{
                scene->I[i].Wm = glm::scale(scene->I[i].Wm, glm::vec3(1.05f, 1.05f, 1.05f));
            }
            
            updateUBO(scene->I[i].Wm, ViewPrj);

            mapToDescriptorSet(scene->DS[i], currentImage);
            
            fireworkIndex = (fireworkIndex + 1) % fullFireworkFrames.size();
        }
    }
    
    void initGUBO(LightsData lightsData){
        gubo.ambientLightDir = glm::vec3(cos(DEG_135), sin(DEG_135), 0.0f);
        gubo.ambientLightColor = ONE_VEC4;
        gubo.eyeDir = ZERO_VEC4;
        gubo.eyeDir.w = 1.0;
        gubo.cosIn = lightsData.cosIn;
        gubo.cosOut = lightsData.cosOut;
    }
    
    void updateGUBO(LightsData lightsData, glm::vec3 cameraPosition) {
        // updates global uniforms
        for (int i = 0; i < LIGHTS_COUNT; i++) {
            gubo.lightColor[i] = glm::vec4(lightsData.lightColors[i], lightsData.lightIntensities[i]);
            gubo.lightDir[i].v = lightsData.lightWorldMatrices[i] * glm::vec4(0, 0, 1, 0);
            gubo.lightPos[i].v = lightsData.lightWorldMatrices[i] * glm::vec4(0, 0, 0, 1);
            gubo.lightOn[i].v = lightsData.lightOn[i];
        }
        gubo.eyePos = cameraPosition;
    }
    
    void updateUBO(glm::mat4 worldMatrix, glm::mat4 viewProjection){
        ubo.mMat = worldMatrix;
        ubo.mvpMat = viewProjection * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
    }
    
    void mapToDescriptorSet(DescriptorSet* DS, int currentImage){
        DS->map(currentImage, &ubo, sizeof(ubo), 0);
        DS->map(currentImage, &gubo, sizeof(gubo), 2);
    }
    
public:
    
    void init(std::vector<void*> params) override {
        
        LightsData lightsData;
        if (params.size() == 2) {
            scene = static_cast<Scene*>(params[0]);
            lightsData = *static_cast<LightsData*>(params[1]);
        } else {
            std::cout << "DrawManager.init(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        
        initGUBO(lightsData);
    }
    
    void update(std::vector<void*> params) override {
        
        int currentImage;
        float pitch;
        float yaw;
        float roll;
        glm::vec3 carPosition;
        glm::vec3 cameraPosition;
        glm::mat4 viewProjection;
        LightsData lightsData;
        
        if (params.size() == 8) {
            currentImage = *static_cast<int*>(params[0]);
            pitch = *static_cast<float*>(params[1]);
            yaw = *static_cast<float*>(params[2]);
            roll = *static_cast<float*>(params[3]);
            carPosition = *static_cast<glm::vec3*>(params[4]);
            cameraPosition = *static_cast<glm::vec3*>(params[5]);
            viewProjection = *static_cast<glm::mat4*>(params[6]);
            lightsData = *static_cast<LightsData*>(params[7]);
        } else {
            std::cout << "DrawManager.update(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        
        updateGUBO(lightsData, cameraPosition);
        
        // draws the car
        drawCar(currentImage, yaw, carPosition, viewProjection, pitch, roll);
        
        // draws the circuit and its fixed decorations
        drawWorld(currentImage, yaw, carPosition, viewProjection);
        
        // draws the dir_barrier that block some path
        drawDirBarrier(currentImage, yaw, carPosition, viewProjection);

        // draws the coins
        drawCoins(currentImage, yaw, carPosition, viewProjection);
        
        // draws the airplane
        drawAirplane(currentImage, yaw, carPosition, viewProjection);
        
        // draws the airship
        drawAirship(currentImage, yaw, carPosition, viewProjection);
        
        // draws the Earth
        drawEarth(currentImage, yaw, carPosition, viewProjection);
        
        // draws the Moon
        drawMoon(currentImage, yaw, carPosition, viewProjection);
        
        // draws the space ships
        drawSpaceShips(currentImage, yaw, carPosition, viewProjection);
        
        // draws the fireworks
        drawFireworks(currentImage, yaw, carPosition, viewProjection);
    }
    
};

void addInstanceToWorld(std::string instance_id){
    world.push_back(instance_id);
}

void addInstanceToCoins(std::string instance_id){
    coins.push_back(instance_id);
}

glm::mat4 getCarTextureWorldMatrix(glm::vec3 bodyPosition, float bodyPitch, float bodyYaw, float bodyRoll){
    float adjustedRoll = std::clamp(bodyRoll, -0.005f, 0.005f);
    return MakeWorld(bodyPosition, bodyYaw, bodyPitch, adjustedRoll) * ONE_MAT4;
}

#endif
