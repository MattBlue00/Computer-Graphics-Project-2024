#ifndef TYPES_HPP
#define TYPES_HPP

#include "Utils.hpp"
#include <btBulletDynamicsCommon.h>

struct LightsData{
    std::vector<glm::mat4> lightWorldMatrices;
    std::vector<glm::vec3> lightColors;
    std::vector<float> lightIntensities;
    std::vector<glm::vec3> lightOn;
    float cosIn;
    float cosOut;
};

struct CameraWorldData {
    float pitch;
    float yaw;
    float roll;
    float distance;
    glm::vec3 position;
    glm::mat4 viewProjection;
};

struct CarWorldData {
    float pitch;
    float yaw;
    float roll;
    glm::vec3 position;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 ambientLightDir;
    alignas(16) glm::vec4 ambientLightColor;
    struct {
        alignas(16) glm::vec3 v;
    } lightDir[LIGHTS_COUNT];
    struct {
        alignas(16) glm::vec3 v;
    } lightPos[LIGHTS_COUNT];
    alignas(16) glm::vec4 lightColor[LIGHTS_COUNT];
    alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec4 eyeDir;
    struct {
        alignas(16) glm::vec3 v;
    } lightOn[LIGHTS_COUNT];
    alignas(4) float cosIn;
    alignas(4) float cosOut;
};

struct AmbientVertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 UV;
};

struct MetalsVertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 UV;
    glm::vec2 props; // x: metalness, y: roughness
};

#endif
