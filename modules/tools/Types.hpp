#ifndef TYPES_HPP
#define TYPES_HPP

#import "Utils.hpp"

struct LightsData{
    std::vector<glm::mat4> lightWorldMatrices;
    std::vector<glm::vec3> lightColors;
    std::vector<float> lightIntensities;
    std::vector<glm::vec3> lightOn;
    float cosIn;
    float cosOut;
};

struct CameraData {
    float CamPitch;
    float CamYaw;
    float CamDist;
    float CamRoll;
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

struct Vertex {
    glm::vec3 pos;
    glm::vec2 UV;
    glm::vec3 norm;
};

#endif
