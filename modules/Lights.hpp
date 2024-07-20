#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include "Utils.hpp"

// lights constants

const std::array<float, 3> LIGHT_COLOR = {0.681277871131897f, 1.0f, 0.9654425978660583f};

// lights variables

glm::mat4 LightWorldMatrices[LIGHTS_COUNT];
glm::vec3 LightColors[LIGHTS_COUNT];
float LightIntensities[LIGHTS_COUNT];
glm::vec4 lightOn;

int totalNumberOfDynamicallyAddedLights = 0;

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
        
        json lightArray = js["lights"];
        
        // DYNAMICALLY ADDS LIGHTS
        
        /*
        // bleachers point lights
        for(int i = 0; i < POINT_LIGHTS_GOAL_BLEACHERS / 2; i++) {
            totalNumberOfDynamicallyAddedLights++;
            lightArray.push_back({
                { "color", { POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", { 11.75, 10, FIRST_BLEACHERS_START + BLEACHERS_STEP * 4 * i} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_l" + std::to_string(totalNumberOfDynamicallyAddedLights) }
            });
            lightArray.push_back({
                { "color", { POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", { -12, 10, FIRST_BLEACHERS_START + BLEACHERS_STEP * 4 * i} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_r" + std::to_string(totalNumberOfDynamicallyAddedLights) }
            });
        }
        
        // pre-rainbow point lights
        for(int i = 0; i < POINT_LIGHTS_PRE_RAINBOW / 2; i++) {
            totalNumberOfDynamicallyAddedLights++;
            lightArray.push_back({
                { "color", { POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", { 11.75, 10, 160 + 40 * i} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_l" + std::to_string(totalNumberOfDynamicallyAddedLights) }
            });
            lightArray.push_back({
                { "color", { POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", { -12, 10, 160 + 40 * i} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_r" + std::to_string(totalNumberOfDynamicallyAddedLights) }
            });
        }
        
        // post-rainbow point lights
        for(int i = 0; i < POINT_LIGHTS_POST_RAINBOW / 2; i++) {
            totalNumberOfDynamicallyAddedLights++;
            lightArray.push_back({
                { "color", { POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", { 11.75, 10, 270 + 60 * i} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_l" + std::to_string(totalNumberOfDynamicallyAddedLights) }
            });
            lightArray.push_back({
                { "color", { POINT_LIGHT_COLOR[0], POINT_LIGHT_COLOR[1], POINT_LIGHT_COLOR[2]} },
                { "translation", { -12, 10, 270 + 60 * i} },
                { "intensity", POINT_LIGHT_INTENSITY },
                { "type", "point" },
                { "name", "light_r" + std::to_string(totalNumberOfDynamicallyAddedLights) }
            });
        }*/
        
        // PREPARES LIGHTS FOR THE APPLICATION
        
        for(int i = 1; i < LIGHTS_COUNT; i++){
            json lightDescription = lightArray[i-1];
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
