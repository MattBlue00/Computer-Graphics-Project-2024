#ifndef DRAW_MANAGER_HPP
#define DRAW_MANAGER_HPP

#include "engine/main/Scene.hpp"
#include "PhysicsManager.hpp"
#include "tools/WVP.hpp"
#include "tools/Types.hpp"
#include "Debug.hpp"
#include "Utils.hpp"

class DrawManager : public Manager {
    
protected:

    PhongUniformBufferObject phongUbo{};
    CookTorranceUniformBufferObject cookTorranceUbo{};
    ToonUniformBufferObject toonUbo{};
    
    GlobalUniformBufferObject gubo{};
    
    void drawGameObjects() {
        for(GameObject* obj : gameObjects){
            obj->update();
            switch (obj->getPipelineType()){
                case PHONG:
                    updatePhongUBO(obj->worldMatrix, cameraWorldData.viewProjection);
                    obj->mapMemoryPhong(EngineCurrentImage, &gubo, &phongUbo);
                    break;
                case COOK_TORRANCE:
                    updateCookTorranceUBO(obj->worldMatrix, cameraWorldData.viewProjection, obj->getProperty("metalness"), obj->getProperty("roughness"));
                    obj->mapMemoryCookTorrance(EngineCurrentImage, &gubo, &cookTorranceUbo);
                    break;
                case TOON:
                    updateToonUBO(obj->worldMatrix, cameraWorldData.viewProjection);
                    obj->mapMemoryToon(EngineCurrentImage, &gubo, &toonUbo);
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
    
    void updatePhongUBO(glm::mat4 worldMatrix, glm::mat4 viewProjection){
        phongUbo.mMat = worldMatrix;
        phongUbo.mvpMat = viewProjection * phongUbo.mMat;
        phongUbo.nMat = glm::inverse(glm::transpose(phongUbo.mMat));
    }
    
    void updateCookTorranceUBO(glm::mat4 worldMatrix, glm::mat4 viewProjection, float metalness, float roughness){
        cookTorranceUbo.mMat = worldMatrix;
        cookTorranceUbo.mvpMat = viewProjection * cookTorranceUbo.mMat;
        cookTorranceUbo.nMat = glm::inverse(glm::transpose(cookTorranceUbo.mMat));
        cookTorranceUbo.metalness = metalness;
        cookTorranceUbo.roughness = roughness;
    }
    
    void updateToonUBO(glm::mat4 worldMatrix, glm::mat4 viewProjection){
        toonUbo.mMat = worldMatrix;
        toonUbo.mvpMat = viewProjection * toonUbo.mMat;
        toonUbo.nMat = glm::inverse(glm::transpose(toonUbo.mMat));
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

#endif
