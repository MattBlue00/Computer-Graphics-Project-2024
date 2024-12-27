#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "engine/main/Scene.hpp"
#include "PhysicsManager.hpp"
#include "tools/WVP.hpp"
#include "tools/Types.hpp"
#include "Utils.hpp"

class DrawManager : public Manager {
    
protected:

    UniformBufferObject ubo{};
    GlobalUniformBufferObject gubo{};
    
    void drawGameObjects() {
        for(GameObject* obj : gameObjects){
            obj->update();
            updateUBO(obj->worldMatrix, cameraWorldData.viewProjection);
            obj->mapMemory(EngineCurrentImage, &gubo, &ubo);
        }
    }
    
    void initGUBO(){
        gubo.ambientLightDir = glm::vec3(cos(DEG_135), sin(DEG_135), 0.0f);
        gubo.ambientLightColor = ONE_VEC4;
        gubo.eyeDir = ZERO_VEC4;
        gubo.eyeDir.w = 1.0;
        gubo.cosIn = lightsData.cosIn;
        gubo.cosOut = lightsData.cosOut;
    }
    
    void updateGUBO() {
        // updates global uniforms
        for (int i = 0; i < LIGHTS_COUNT; i++) {
            gubo.lightColor[i] = glm::vec4(lightsData.lightColors[i], lightsData.lightIntensities[i]);
            gubo.lightDir[i].v = lightsData.lightWorldMatrices[i] * glm::vec4(0, 0, 1, 0);
            gubo.lightPos[i].v = lightsData.lightWorldMatrices[i] * glm::vec4(0, 0, 0, 1);
            gubo.lightOn[i].v = lightsData.lightOn[i];
        }
        gubo.eyePos = cameraWorldData.position;
    }
    
    void updateUBO(glm::mat4 worldMatrix, glm::mat4 viewProjection){
        ubo.mMat = worldMatrix;
        ubo.mvpMat = viewProjection * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
    }
    
public:
    
    void init() override {
        initGUBO();
    }
    
    void update() override {
        updateGUBO();
        drawGameObjects();
    }
    
    void cleanup() override {}
    
};

glm::mat4 getCarTextureWorldMatrix(glm::vec3 bodyPosition, float bodyPitch, float bodyYaw, float bodyRoll){
    float adjustedRoll = std::clamp(bodyRoll, -0.005f, 0.005f);
    return MakeWorld(bodyPosition, bodyYaw, bodyPitch, adjustedRoll) * ONE_MAT4;
}

#endif
