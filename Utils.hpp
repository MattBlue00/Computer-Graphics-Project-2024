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
const float DEG_5   = glm::radians(5.0f);
const float DEG_20  = glm::radians(20.0f);
const float DEG_30  = glm::radians(30.0f);
const float DEG_35  = glm::radians(35.0f);
const float DEG_45  = glm::radians(45.0f);
const float DEG_90  = glm::radians(90.0f);
const float DEG_120 = glm::radians(120.0f);
const float DEG_135 = glm::radians(135.0f);

// APP-SPECIFIC CONSTANTS

// scene ids
const int THIRD_PERSON_SCENE = 0;
const int FIRST_PERSON_SCENE = 1;

// total lights count
const int POINT_LIGHTS_BLEACHERS = 30;
const int POINT_LIGHTS_RAINBOW = 3;
const int POINT_LIGHTS_COUNT = POINT_LIGHTS_BLEACHERS + POINT_LIGHTS_RAINBOW;
const int DIR_LIGHTS_COUNT = 0;
const int LIGHTS_COUNT = POINT_LIGHTS_COUNT + DIR_LIGHTS_COUNT;

// GENERAL STRUCTS

// ubo
struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

// gubo
struct GlobalUniformBufferObject {
    struct {
        alignas(16) glm::vec3 v;
    } lightDir[LIGHTS_COUNT];
    struct {
        alignas(16) glm::vec3 v;
    } lightPos[LIGHTS_COUNT];
    alignas(16) glm::vec4 lightColor[LIGHTS_COUNT];
    alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec4 eyeDir;
    alignas(16) glm::vec4 lightOn;
};

// vertex
struct Vertex {
    glm::vec3 pos;
    glm::vec2 UV;
    glm::vec3 norm;
};

// PROJECT-SPECIFIC FUNCTIONS

std::string getProjectPath() {
    json js;
    std::ifstream ifs("config.json");
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

    return js["project_path"].get<std::string>();
}

#endif
