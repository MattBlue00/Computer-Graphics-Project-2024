#ifndef MANAGER_UPDATE_DATA_HPP
#define MANAGER_UPDATE_DATA_HPP

#include "engine/main/utils/PositionData.hpp"

struct AudioManagerUpdateData: ManagerUpdateData{};

struct CameraManagerUpdateData: ManagerUpdateData{
    float aspectRatio;
    float deltaT;
    glm::vec3 cameraRotationInput;
    glm::vec3 carMovementInput;
    PositionData carPosition;
    
    CameraManagerUpdateData(float ar, float dt, glm::vec3 cri, glm::vec3 cmi, PositionData cp)
    : aspectRatio(ar), deltaT(dt), cameraRotationInput(cri), carMovementInput(cmi), carPosition(cp) {}
};

struct CarManagerUpdateData: ManagerUpdateData{
    glm::vec3 carMovementInput;
    
    CarManagerUpdateData(glm::vec3 cmi) : carMovementInput(cmi) {}
};

struct DrawManagerUpdateData: ManagerUpdateData{
    int currentImage;
    PositionData positionData;
    glm::vec3 cameraPosition;
    glm::mat4 viewProjection;
    LightsData lightsData;
    
    DrawManagerUpdateData(int ci, PositionData pd, glm::vec3 cp, glm::mat4 vp, LightsData ld)
    : currentImage(ci), positionData(pd), cameraPosition(cp), viewProjection(vp), lightsData(ld) {}
};

struct InputManagerUpdateData: ManagerUpdateData{
    bool* shouldRebuildPipeline;
    bool* headlightsChanged;
    bool* sceneChanged;
    bool* viewReset;
    
    InputManagerUpdateData(bool* srp, bool* hc, bool* sc, bool* vr)
    : shouldRebuildPipeline(srp), headlightsChanged(hc), sceneChanged(sc), viewReset(vr) {}
};

struct LightsManagerUpdateData: ManagerUpdateData{
    glm::mat4 textureWm;
    
    LightsManagerUpdateData(glm::mat4 tw) : textureWm(tw) {}
};

struct PhysicsManagerUpdateData: ManagerUpdateData{
    float deltaTime;
    btRaycastVehicle* vehicle;
    
    PhysicsManagerUpdateData(float dt, btRaycastVehicle* v)
    : deltaTime(dt), vehicle(v) {}
};

struct SceneManagerUpdateData: ManagerUpdateData{};

struct UIManagerUpdateData: ManagerUpdateData{};

#endif

