#ifndef LIGHTS_MANAGER_HPP
#define LIGHTS_MANAGER_HPP

#include "Utils.hpp"
#include "utils/ManagerInitData.hpp"
#include "utils/ManagerUpdateData.hpp"

struct LightsManager : public Observer, public Manager {
    
protected:
    
    LightsData lightsData{};
    json lightsArray;
    bool isLightInit = false;

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
        if (index != -1) {
            glm::vec3 rightLocalTranslation = glm::vec3(lightsArray[index]["translation"][0],
                                                        lightsArray[index]["translation"][1],
                                                        lightsArray[index]["translation"][2]);

            lightsData.lightWorldMatrices[index] = glm::translate(textureWm, rightLocalTranslation);
        } else {
            std::cout << "Light not found!" << std::endl;
        }
    }
    
public:
    
    // inits light system
    void init(ManagerInitData* param) override {
        
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
            
            isLightInit = true;
            
        } catch (const nlohmann::json::exception &e) {
            std::cout << e.what() << '\n';
        }
        
        lightsData.cosIn = 0.985f; // cosine of 10 degrees
        lightsData.cosOut = 0.906f; // cosine of 25 degrees
    }
    
    // update car light position based on car position
    void update(ManagerUpdateData* param) override {
        
        if(!isLightInit) return;
        
        auto* data = dynamic_cast<LightsManagerUpdateData*>(param);
        
        if (!data) {
            throw std::runtime_error("Invalid type for ManagerUpdateData");
        }

        updateLightWorldMatrix(getLightIndexByName("brake_light_left"), data->textureWm);
        updateLightWorldMatrix(getLightIndexByName("brake_light_right"), data->textureWm);
        updateLightWorldMatrix(getLightIndexByName("headlight_left"), data->textureWm);
        updateLightWorldMatrix(getLightIndexByName("headlight_right"), data->textureWm);

    }
    
    void cleanup() override {}
    
    LightsData getLightsData(){
        return lightsData;
    }
    
    //------Observer methods--------
    
    void onStartSemaphore(int countDownValue) override {
        
        resetSemaphore();
        
        switch (countDownValue) {
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
                break;
            default:
                break;
        }
        
    }
    
    void onBrakeActive(bool isBrakeActive) override {
        int leftIndex = getLightIndexByName("brake_light_left");
        int rightIndex = getLightIndexByName("brake_light_right");
        
        lightsData.lightOn[leftIndex] = isBrakeActive? ONE_VEC3 : ZERO_VEC3;
        lightsData.lightOn[rightIndex] = isBrakeActive? ONE_VEC3 : ZERO_VEC3;
        
    }
    
    void onChangeHeadlightsStatus() override {
        int leftIndex = getLightIndexByName("headlight_left");
        int rightIndex = getLightIndexByName("headlight_right");
        
        glm::vec3 currentLeftLightStatus = lightsData.lightOn[leftIndex];
        glm::vec3 currentRightLightStatus = lightsData.lightOn[leftIndex];
        
        lightsData.lightOn[leftIndex] = currentLeftLightStatus == ZERO_VEC3 ? ONE_VEC3 : ZERO_VEC3;
        lightsData.lightOn[rightIndex] = currentRightLightStatus == ZERO_VEC3 ? ONE_VEC3 : ZERO_VEC3;
    }

};

#endif

