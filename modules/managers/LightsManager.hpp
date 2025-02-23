#ifndef LIGHTS_MANAGER_HPP
#define LIGHTS_MANAGER_HPP

#include "Utils.hpp"
#include "../modules/engine/pattern/Receiver.hpp"

class LightsManager : public Manager, public Receiver {
    
protected:
    
    glm::vec3 reverseLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f);
    
    json lightsArray;
    
    int waitHeadlights = 60;
    
    int _leftBrakeLightIndex;
    int _rightBrakeLightIndex;
    int _leftHeadlightIndex;
    int _rightHeadlightIndex;
    int _airplaneHeadlightIndex;
    int _spaceship1HeadlightIndex;
    int _spaceship2HeadlightIndex;
    int _spaceship3HeadlightIndex;
    
    int _airplaneObjectIndex;
    int _spaceship1ObjectIndex;
    int _spaceship2ObjectIndex;
    int _spaceship3ObjectIndex;
    
    bool semaphoreGreenLightOn = false;
    bool didUpdateBrakeLights = false;

    void resetSemaphore(){
        lightsData.lightOn[getLightIndexByName("red_light_left")] = ZERO_VEC3;
        lightsData.lightOn[getLightIndexByName("red_light_right")] = ZERO_VEC3;
        lightsData.lightOn[getLightIndexByName("yellow_light_left")] = ZERO_VEC3;
        lightsData.lightOn[getLightIndexByName("yellow_light_right")] = ZERO_VEC3;
        lightsData.lightOn[getLightIndexByName("green_light_left")] = ZERO_VEC3;
        lightsData.lightOn[getLightIndexByName("green_light_right")] = ZERO_VEC3;
    }
    
    // Returns the index of the light with the specified name
    int getLightIndexByName(const std::string& lightName) const {
        auto findLightIndex = [lightName, this]() -> int {
            auto it = std::find_if(lightsArray.begin(), lightsArray.end(), [&lightName](const json& light) {
                return light["name"] == lightName;
            });
            
            if (it != lightsArray.end()) {
                return (int)std::distance(lightsArray.begin(), it);
            } else {
                return -1; // Return -1 if light is not found
            }
        };

        return findLightIndex();
    }
    
    void updateLightWorldMatrix(const int index, glm::mat4 textureWm){
        glm::vec3 rightLocalTranslation = glm::vec3(lightsArray[index]["translation"][0],
                                                    lightsArray[index]["translation"][1],
                                                    lightsArray[index]["translation"][2]);

        lightsData.lightWorldMatrices[index] = glm::translate(textureWm, rightLocalTranslation);
        if(index == _airplaneHeadlightIndex){
            lightsData.lightWorldMatrices[index] = glm::rotate(lightsData.lightWorldMatrices[index], DEG_60, X_AXIS);
        }
    }
    
    void setSemaphore(int countdownValue) {
        
        resetSemaphore();
        
        switch (countdownValue) {
            case 10:
            case 9:
            case 8:
            case 7:
            case 6:
            case 5:
            case 4:
            case 3:
                lightsData.lightOn[getLightIndexByName("red_light_left")] = ONE_VEC3;
                lightsData.lightOn[getLightIndexByName("red_light_right")] = ONE_VEC3;
                break;
            case 2:
                lightsData.lightOn[getLightIndexByName("red_light_left")] = ONE_VEC3;
                lightsData.lightOn[getLightIndexByName("red_light_right")] = ONE_VEC3;
                lightsData.lightOn[getLightIndexByName("yellow_light_left")] = ONE_VEC3;
                lightsData.lightOn[getLightIndexByName("yellow_light_right")] = ONE_VEC3;
                break;
            case 1:
                lightsData.lightOn[getLightIndexByName("green_light_left")] = ONE_VEC3;
                lightsData.lightOn[getLightIndexByName("green_light_right")] = ONE_VEC3;
                semaphoreGreenLightOn = true;
                break;
            default:
                lightsData.lightOn[getLightIndexByName("green_light_left")] = ONE_VEC3;
                lightsData.lightOn[getLightIndexByName("green_light_right")] = ONE_VEC3;
                break;
        }
        
    }
    
    void onCountdown(int countdownValue) {
        if(countdownValue > 0){
            setSemaphore(countdownValue);
        }
        else{
            lightsData.lightOn[_leftBrakeLightIndex] = ZERO_VEC3;
            lightsData.lightOn[_rightBrakeLightIndex] = ZERO_VEC3;
        }
    }
    
    void onBrake() {
        lightsData.lightOn[_leftBrakeLightIndex] = ONE_VEC3;
        lightsData.lightOn[_rightBrakeLightIndex] = ONE_VEC3;
        didUpdateBrakeLights = true;
    }
    
    void onHeadlightsStatusChange() {
        if(waitHeadlights >= 60){
            glm::vec3 currentLeftLightStatus = lightsData.lightOn[_leftHeadlightIndex];
            glm::vec3 currentRightLightStatus = lightsData.lightOn[_rightHeadlightIndex];
            
            lightsData.lightOn[_leftHeadlightIndex] = currentLeftLightStatus == ZERO_VEC3 ? ONE_VEC3 : ZERO_VEC3;
            lightsData.lightOn[_rightHeadlightIndex] = currentRightLightStatus == ZERO_VEC3 ? ONE_VEC3 : ZERO_VEC3;
            
            waitHeadlights = 0;
        }
    }
    
    void onReverse(){
        lightsData.lightColors[_leftBrakeLightIndex] = reverseLightColor;
        lightsData.lightColors[_rightBrakeLightIndex] = reverseLightColor;
        didUpdateBrakeLights = true;
    }
    
public:
    
    // inits light system
    void init() override {
        
        // LOADS FILE
        json js;
        std::ifstream ifs("models/lights.json");
        if (!ifs.is_open()) {
            std::cout << "Error! Lights file not found!";
            exit(-1);
        }
        
        try {
            // PARSES FILE
            std::cout << "Parsing lights.json\n";
            ifs >> js;
            ifs.close();
            
            lightsArray = js["lights"];
            
            // Resize vectors to hold the lights data
            lightsData.lightWorldMatrices.resize(LIGHTS_COUNT);
            lightsData.lightColors.resize(LIGHTS_COUNT);
            lightsData.lightIntensities.resize(LIGHTS_COUNT);
            lightsData.lightOn.resize(LIGHTS_COUNT);
            
            // PREPARES LIGHTS FOR THE APPLICATION
            for (int i = 0; i < LIGHTS_COUNT; i++) {
                json lightDescription = lightsArray[i];
                glm::vec3 lightTranslation;
                glm::vec3 lightScale;
                glm::quat quaternion;
                if (lightDescription.contains("translation")) {
                    lightTranslation = glm::vec3(lightDescription["translation"][0],
                                                 lightDescription["translation"][1],
                                                 lightDescription["translation"][2]);
                } else {
                    lightTranslation = ZERO_VEC3;
                }
                if (lightDescription.contains("rotation")) {
                    quaternion = glm::quat(lightDescription["rotation"][3],
                                           lightDescription["rotation"][0],
                                           lightDescription["rotation"][1],
                                           lightDescription["rotation"][2]);
                } else {
                    quaternion = ONE_QUAT;
                }
                if (lightDescription.contains("scale")) {
                    lightScale = glm::vec3(lightDescription["scale"][0],
                                           lightDescription["scale"][1],
                                           lightDescription["scale"][2]);
                } else {
                    lightScale = ONE_VEC3;
                }
                
                lightsData.lightWorldMatrices[i] = glm::translate(ONE_MAT4, lightTranslation) *
                glm::mat4(quaternion) *
                glm::scale(ONE_MAT4, lightScale);
                
                json lightColor = lightDescription["color"];
                lightsData.lightColors[i] = glm::vec3(lightColor[0], lightColor[1], lightColor[2]);
                
                lightsData.lightIntensities[i] = lightDescription["intensity"];
                lightsData.lightOn[i] = ONE_VEC3;
            }
            
        } catch (const nlohmann::json::exception &e) {
            std::cout << e.what() << '\n';
        }
        
        lightsData.cosIn = 0.985f; // cosine of 10 degrees
        lightsData.cosOut = 0.906f; // cosine of 25 degrees
        
        _leftBrakeLightIndex = getLightIndexByName("brake_light_left");
        _rightBrakeLightIndex = getLightIndexByName("brake_light_right");
        _leftHeadlightIndex = getLightIndexByName("headlight_left");
        _rightHeadlightIndex = getLightIndexByName("headlight_right");
        _airplaneHeadlightIndex = getLightIndexByName("airplane_headlight");
        _spaceship1HeadlightIndex = getLightIndexByName("spaceship_1_headlight");
        _spaceship2HeadlightIndex = getLightIndexByName("spaceship_2_headlight");
        _spaceship3HeadlightIndex = getLightIndexByName("spaceship_3_headlight");
        
        _airplaneObjectIndex = 0;
        _spaceship1ObjectIndex = 0;
        _spaceship2ObjectIndex = 0;
        _spaceship3ObjectIndex = 0;
        
        int index = 0;
        for(GameObject* obj : gameObjects){
            if(obj->getId() == ("airplane")){
                _airplaneObjectIndex = index;
            }
            else if(obj->getId() == ("spaceship_1")){
                _spaceship1ObjectIndex = index;
            }
            else if(obj->getId() == ("spaceship_2")){
                _spaceship2ObjectIndex = index;
            }
            else if(obj->getId() == ("spaceship_3")){
                _spaceship3ObjectIndex = index;
            }
            index++;
        }
    }
    
    // update car light position based on car position
    void update() override {
        updateLightWorldMatrix(_leftBrakeLightIndex, vehicleTextureWorldMatrix);
        updateLightWorldMatrix(_rightBrakeLightIndex, vehicleTextureWorldMatrix);
        updateLightWorldMatrix(_leftHeadlightIndex, vehicleTextureWorldMatrix);
        updateLightWorldMatrix(_rightHeadlightIndex, vehicleTextureWorldMatrix);
        updateLightWorldMatrix(_airplaneHeadlightIndex, gameObjects[_airplaneObjectIndex]->worldMatrix);
        updateLightWorldMatrix(_spaceship1HeadlightIndex, gameObjects[_spaceship1ObjectIndex]->worldMatrix);
        updateLightWorldMatrix(_spaceship2HeadlightIndex, gameObjects[_spaceship2ObjectIndex]->worldMatrix);
        updateLightWorldMatrix(_spaceship3HeadlightIndex, gameObjects[_spaceship3ObjectIndex]->worldMatrix);
        
        if(waitHeadlights < 60){
            waitHeadlights++;
        }
        
        if(!didUpdateBrakeLights && semaphoreGreenLightOn){
            lightsData.lightOn[_leftBrakeLightIndex] = ZERO_VEC3;
            lightsData.lightOn[_rightBrakeLightIndex] = ZERO_VEC3;
            lightsData.lightColors[_leftBrakeLightIndex] = redColor;
            lightsData.lightColors[_rightBrakeLightIndex] = redColor;
        }
        didUpdateBrakeLights = false;
    }
    
    void cleanup() override {}
    
    void onSignal(std::string id, std::any data) override {
        if (id == COUNTDOWN_SIGNAL) {
            onCountdown(std::any_cast<int>(data));
        } else if (id == BRAKE_SIGNAL) {
            onBrake();
        } else if (id == HEADLIGHTS_CHANGE_SIGNAL) {
            onHeadlightsStatusChange();
        } else if (id == REVERSE_SIGNAL) {
            onReverse();
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }

};

#endif

