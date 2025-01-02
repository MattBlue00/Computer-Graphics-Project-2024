#ifndef UTILS_HPP
#define UTILS_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

// TYPE ALIASES

using json = nlohmann::json;

// GENERAL CONSTANTS

// notable vectors
const glm::vec3 ZERO_VEC3   = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 ONE_VEC3    = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 X_AXIS      = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 Y_AXIS      = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Z_AXIS      = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec4 ZERO_VEC4   = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
const glm::vec4 ONE_VEC4    = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

// notable matrices
const glm::mat4 ONE_MAT4    = glm::mat4(1.0f);

// notable quaternions
const glm::quat ONE_QUAT    = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

// notable angles
const float DEG_0   = glm::radians(0.0f);
const float DEG_0_2 = glm::radians(0.2f);
const float DEG_2_5 = glm::radians(2.5f);
const float DEG_5   = glm::radians(5.0f);
const float DEG_20  = glm::radians(20.0f);
const float DEG_30  = glm::radians(30.0f);
const float DEG_35  = glm::radians(35.0f);
const float DEG_45  = glm::radians(45.0f);
const float DEG_60  = glm::radians(60.0f);
const float DEG_90  = glm::radians(90.0f);
const float DEG_120 = glm::radians(120.0f);
const float DEG_135 = glm::radians(135.0f);

// APP-SPECIFIC CONSTANTS

// scene ids
const int THIRD_PERSON_SCENE = 0;
const int FIRST_PERSON_SCENE = 1;

// total lights count

const int LIGHTS_COUNT = 11;

// PROJECT-SPECIFIC FUNCTIONS

json parseConfigFile() {
    json js;
    std::ifstream ifs("config/config.json");
    if (!ifs.is_open()) {
        std::cerr << "Error! Config file not found!" << std::endl;
        exit(-1);
    }
    
    try {
        std::cout << "Parsing config.json" << std::endl;
        ifs >> js;
        ifs.close();
    }
    catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        exit(-1);
    }

    return js;
}

#endif
