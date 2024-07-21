#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include "Utils.hpp"
#include "Car.hpp"

// lights variables
struct LightManager : public Observer {
    std::vector<glm::mat4> LightWorldMatrices;
    std::vector<glm::vec3> LightColors;
    std::vector<float> LightIntensities;
    std::vector<glm::vec3> LightOn;
    json lightArray;
    bool isLightInit = false;
    
    // inits light system
    void initLights() {
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
            
            lightArray = js["lights"];
            int lightsCount = (int) lightArray.size();
            
            // Resize vectors to hold the lights data
            LightWorldMatrices.resize(lightsCount);
            LightColors.resize(lightsCount);
            LightIntensities.resize(lightsCount);
            LightOn.resize(lightsCount);
            
            // PREPARES LIGHTS FOR THE APPLICATION
            for (int i = 0; i < lightsCount; i++) {
                json lightDescription = lightArray[i];
                glm::vec3 LightTranslation;
                glm::vec3 LightScale;
                glm::quat Quaternion;
                if (lightDescription.contains("translation")) {
                    LightTranslation = glm::vec3(lightDescription["translation"][0],
                                                 lightDescription["translation"][1],
                                                 lightDescription["translation"][2]);
                } else {
                    LightTranslation = ZERO_VEC3;
                }
                if (lightDescription.contains("rotation")) {
                    Quaternion = glm::quat(lightDescription["rotation"][3],
                                           lightDescription["rotation"][0],
                                           lightDescription["rotation"][1],
                                           lightDescription["rotation"][2]);
                } else {
                    Quaternion = ONE_QUAT;
                }
                if (lightDescription.contains("scale")) {
                    LightScale = glm::vec3(lightDescription["scale"][0],
                                           lightDescription["scale"][1],
                                           lightDescription["scale"][2]);
                } else {
                    LightScale = ONE_VEC3;
                }
                
                LightWorldMatrices[i] = glm::translate(ONE_MAT4, LightTranslation) *
                glm::mat4(Quaternion) *
                glm::scale(ONE_MAT4, LightScale);
                
                json LightColor = lightDescription["color"];
                LightColors[i] = glm::vec3(LightColor[0], LightColor[1], LightColor[2]);
                
                LightIntensities[i] = lightDescription["intensity"];
                LightOn[i] = ONE_VEC3;
            }
            
            isLightInit = true;
            
        } catch (const nlohmann::json::exception &e) {
            std::cout << e.what() << '\n';
        }
    }
    
    // update car light position based on car position
    void updateLightPosition() {
        // TO BE IMPLEMENTED
    }
    
    void resetSemaphore(){
        int i1 =  getLightIndexByName("red_light_left");
        int i2 = getLightIndexByName("red_light_right");
        int i3 =  getLightIndexByName("yellow_light_left");
        int i4 =  getLightIndexByName("yellow_light_right");
        int i5 = getLightIndexByName("green_light_left");
        int i6 = getLightIndexByName("green_light_right");
        LightOn[i1] = ZERO_VEC3;
        LightOn[i2] = ZERO_VEC3;
        LightOn[i3] = ZERO_VEC3;
        LightOn[i4] = ZERO_VEC3;
        LightOn[i5] = ZERO_VEC3;
        LightOn[i6] = ZERO_VEC3;
    }
    
    void onStartSemaphore(int countDownValue) override {
        
        int idL, idR;
        resetSemaphore();
        
        switch (countDownValue) {
            case 3:
                idL =  getLightIndexByName("red_light_left");
                idR = getLightIndexByName("red_light_right");
                LightOn[idL] = ONE_VEC3;
                LightOn[idR] = ONE_VEC3;
                break;
            case 2:
                idL =  getLightIndexByName("yellow_light_left");
                idR =  getLightIndexByName("yellow_light_right");
                LightOn[idL] = ONE_VEC3;
                LightOn[idR] = ONE_VEC3;
                break;
            case 1:
                idL = getLightIndexByName("green_light_left");
                idR = getLightIndexByName("green_light_right");
                LightOn[idL] = ONE_VEC3;
                LightOn[idR] = ONE_VEC3;
                break;
            default:
                std::cout << "Ignore" << std::endl;
                break;
        }
    }
    
    // Returns the index of the light with the specified name
    int getLightIndexByName(const std::string& lightName) const {
        auto findLightIndex = [lightName, this]() -> int {
            auto it = std::find_if(lightArray.begin(), lightArray.end(), [&lightName](const json& light) {
                return light["name"] == lightName;
            });
            
            if (it != lightArray.end()) {
                return std::distance(lightArray.begin(), it);
            } else {
                return -1; // Return -1 if light is not found
            }
        };

        return findLightIndex();
    }
};

#endif

