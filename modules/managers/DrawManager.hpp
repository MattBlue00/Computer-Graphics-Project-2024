#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "engine/main/Scene.hpp"
#include "PhysicsManager.hpp"
#include "tools/WVP.hpp"
#include "tools/Types.hpp"
#include "Utils.hpp"

class DrawManager : public Manager {
    
protected:

    AmbientUniformBufferObject ambientUbo{};
    MetalsUniformBufferObject metalsUbo{};
    GlobalUniformBufferObject gubo{};
    
    void drawGameObjects() {
        for(GameObject* obj : gameObjects){
            obj->update();
            switch (obj->getPipelineType()){
                case AMBIENT:
                    updateAmbientUBO(obj->worldMatrix, cameraWorldData.viewProjection);
                    obj->mapMemory(EngineCurrentImage, &gubo, &ambientUbo, sizeof(ambientUbo));
                    break;
                case METALS:
                    updateMetalsUBO(obj->worldMatrix, cameraWorldData.viewProjection, obj->getProperty("metalness"), obj->getProperty("roughness"));
                    obj->mapMemory(EngineCurrentImage, &gubo, &metalsUbo, sizeof(metalsUbo));
                    break;
            }
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
    
    void updateAmbientUBO(glm::mat4 worldMatrix, glm::mat4 viewProjection){
        ambientUbo.mMat = worldMatrix;
        ambientUbo.mvpMat = viewProjection * ambientUbo.mMat;
        ambientUbo.nMat = glm::inverse(glm::transpose(ambientUbo.mMat));
    }
    
    void updateMetalsUBO(glm::mat4 worldMatrix, glm::mat4 viewProjection, float metalness, float roughness){
        metalsUbo.mMat = worldMatrix;
        metalsUbo.mvpMat = viewProjection * metalsUbo.mMat;
        metalsUbo.nMat = glm::inverse(glm::transpose(metalsUbo.mMat));
        metalsUbo.metalness = metalness;
        metalsUbo.roughness = roughness;
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
