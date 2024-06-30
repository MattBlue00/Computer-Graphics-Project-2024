#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#ifndef UTILS_HPP
#define UTILS_HPP

#include <glm/glm.hpp>

// GENERAL CONSTANTS

// notable vectors
const glm::vec3 ZERO_VEC3   = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 X_AXIS      = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 Y_AXIS      = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Z_AXIS      = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec4 ZERO_VEC4   = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
const glm::vec4 ONE_VEC4    = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

// notable matrices
const glm::mat4 ONE_MAT4    = glm::mat4(1.0f);

// notable angles
const float DEG_0   = glm::radians(0.0f);
const float DEG_20  = glm::radians(20.0f);
const float DEG_30  = glm::radians(30.0f);
const float DEG_35  = glm::radians(35.0f);
const float DEG_90  = glm::radians(90.0f);
const float DEG_120 = glm::radians(120.0f);
const float DEG_135 = glm::radians(135.0f);

// car parameters
const float STEERING_SPEED = DEG_30;
const float MAX_STEERING_ANGLE = DEG_35;
const float MOVE_SPEED = 100.0f;

// APP-SPECIFIC CONSTANTS

// scene ids
const int THIRD_PERSON_SCENE = 0;
const int FIRST_PERSON_SCENE = 1;

// GENERAL STRUCTS

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec4 lightColor;
    alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec4 eyeDir;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec2 UV;
    glm::vec3 norm;
};

#endif
