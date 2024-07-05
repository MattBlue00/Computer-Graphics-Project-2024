#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include "Utils.hpp"

// lights constants

const std::array<float, 3> POINT_LIGHT_COLOR = {0.681277871131897f, 1.0f, 0.9654425978660583f};
const float POINT_LIGHT_INTENSITY = 12.0f;

// lights variables

glm::mat4 LightWorldMatrices[LIGHTS_COUNT];
glm::vec3 LightColors[LIGHTS_COUNT];
float LightIntensities[LIGHTS_COUNT];
glm::vec4 lightOn;


// reads lights.json file, loads file istances and adds dynamically more instances
void initLights(){
    
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
        
        // DYNAMICALLY ADDS LIGHTS
        
        json lightArray = js["lights"];
        
        // bleachers point lights
        for(int i = 1; i <= POINT_LIGHTS_BLEACHERS / 2; i++) {
            lightArray.push_back({
                { "color", {POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", {15, 5, FIRST_BLEACHERS_START + BLEACHERS_STEP * 2 * (i-1)} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_l" + std::to_string(i) }
            });
            lightArray.push_back({
                { "color", {POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", {-15, 5, FIRST_BLEACHERS_START + BLEACHERS_STEP * 2 * (i-1)} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_r" + std::to_string(i) }
            });
        }
        
        // PREPARES LIGHTS FOR THE APPLICATION
        
        for(int i = 0; i < LIGHTS_COUNT; i++){
            json lightDescription = lightArray[i];
            glm::vec3 LightTranslation;
            glm::vec3 LightScale;
            glm::quat Quaternion;
            if(lightDescription.contains("translation")) {
                LightTranslation = glm::vec3(lightDescription["translation"][0],
                                             lightDescription["translation"][1],
                                             lightDescription["translation"][2]);
            } else {
                LightTranslation = ZERO_VEC3;
            }
            if(lightDescription.contains("rotation")) {
                Quaternion = glm::quat(lightDescription["rotation"][3],
                                       lightDescription["rotation"][0],
                                       lightDescription["rotation"][1],
                                       lightDescription["rotation"][2]);
            } else {
                Quaternion = ONE_QUAT;
            }
            if(lightDescription.contains("scale")) {
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
        }
        
    } catch (const nlohmann::json::exception& e) {
        std::cout << e.what() << '\n';
    }

    lightOn = ONE_VEC4;
}

#endif
