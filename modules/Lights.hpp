#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include "Utils.hpp"
#include "Car.hpp"

glm::quat clampRoll(const glm::quat& rotation);

// lights variables
struct LightManager : public Observer {
    std::vector<glm::mat4> LightWorldMatrices;
    std::vector<glm::vec3> LightColors;
    std::vector<float> LightIntensities;
    std::vector<glm::vec3> LightOn;
    json lightArray;
    bool isLightInit = false;
    float ScosIn = 0.985f; // cosine of 10 degrees
    float ScosOut = 0.906f; // cosine of 25 degrees
    
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
            
            // Resize vectors to hold the lights data
            LightWorldMatrices.resize(LIGHTS_COUNT);
            LightColors.resize(LIGHTS_COUNT);
            LightIntensities.resize(LIGHTS_COUNT);
            LightOn.resize(LIGHTS_COUNT);
            
            // PREPARES LIGHTS FOR THE APPLICATION
            for (int i = 0; i < LIGHTS_COUNT; i++) {
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
    void updateLightPositions() {
        if(!isLightInit) return;
        
        glm::vec3 carPosition = getVehiclePosition(vehicle);
        glm::quat carRotation = getVehicleRotation(vehicle);

        updateLightWorldMatrix(getLightIndexByName("brake_light_left"), carPosition, carRotation);
        updateLightWorldMatrix(getLightIndexByName("brake_light_right"), carPosition, carRotation);
        updateLightWorldMatrix(getLightIndexByName("headlight_left"), carPosition, carRotation);
        updateLightWorldMatrix(getLightIndexByName("headlight_right"), carPosition, carRotation);

    }

    void resetSemaphore(){
        LightOn[getLightIndexByName("red_light_left")] = ZERO_VEC3;
        LightOn[getLightIndexByName("red_light_right")] = ZERO_VEC3;
        LightOn[getLightIndexByName("yellow_light_left")] = ZERO_VEC3;
        LightOn[getLightIndexByName("yellow_light_right")] = ZERO_VEC3;
        LightOn[getLightIndexByName("green_light_left")] = ZERO_VEC3;
        LightOn[getLightIndexByName("green_light_right")] = ZERO_VEC3;
    }
    
    // Returns the index of the light with the specified name
    int getLightIndexByName(const std::string& lightName) const {
        auto findLightIndex = [lightName, this]() -> int {
            auto it = std::find_if(lightArray.begin(), lightArray.end(), [&lightName](const json& light) {
                return light["name"] == lightName;
            });
            
            if (it != lightArray.end()) {
                return (int)std::distance(lightArray.begin(), it);
            } else {
                return -1; // Return -1 if light is not found
            }
        };

        return findLightIndex();
    }
    
    void updateLightWorldMatrix(const int index, glm::vec3 carPosition, glm::quat carRotation){
        if (index != -1) {
            glm::vec3 rightLocalTranslation = glm::vec3(lightArray[index]["translation"][0],
                                                        lightArray[index]["translation"][1],
                                                        lightArray[index]["translation"][2]);
            glm::vec3 rightWorldTranslation = carPosition + carRotation * rightLocalTranslation;

            LightWorldMatrices[index] = glm::translate(ONE_MAT4, rightWorldTranslation) *
                                             glm::mat4(carRotation);
        } else {
            std::cout << "Light headlight_right not found!" << std::endl;
        }
    }
    
    //------Observer methods--------
    
    void onStartSemaphore(int countDownValue) override {
        
        resetSemaphore();
        
        switch (countDownValue) {
            case 6:
            case 5:
            case 4:
            case 3:
                LightOn[getLightIndexByName("red_light_left")] = ONE_VEC3;
                LightOn[getLightIndexByName("red_light_right")] = ONE_VEC3;
                break;
            case 2:
                LightOn[getLightIndexByName("red_light_left")] = ONE_VEC3;
                LightOn[getLightIndexByName("red_light_right")] = ONE_VEC3;
                LightOn[getLightIndexByName("yellow_light_left")] = ONE_VEC3;
                LightOn[getLightIndexByName("yellow_light_right")] = ONE_VEC3;
                break;
            case 1:
                LightOn[getLightIndexByName("green_light_left")] = ONE_VEC3;
                LightOn[getLightIndexByName("green_light_right")] = ONE_VEC3;
                break;
            default:
                break;
        }
        
    }
    
    void onBrakeActive(bool isBrakeActive) override {
        int leftIndex = getLightIndexByName("brake_light_left");
        int rightIndex = getLightIndexByName("brake_light_right");
        
        LightOn[leftIndex] = isBrakeActive? ONE_VEC3 : ZERO_VEC3;
        LightOn[rightIndex] = isBrakeActive? ONE_VEC3 : ZERO_VEC3;
        
    }
};

glm::quat clampRoll(const glm::quat& rotation) {
    // Converti il quaternione in angoli di Eulero
    glm::vec3 euler = glm::eulerAngles(rotation);

    // Clamp dell'angolo di roll (che corrisponde a euler.z)
    euler.z = glm::clamp(euler.z, -0.005f, 0.005f);

    // Riconverti gli angoli di Eulero modificati in un quaternione
    return glm::quat(euler);
}

#endif

