#ifndef MANAGER_INIT_DATA_HPP
#define MANAGER_INIT_DATA_HPP

#include "engine/main/GameObject.hpp"

struct AudioManagerInitData: ManagerInitData{
    json audioMap;
    
    AudioManagerInitData(json am) : audioMap(am) {}
};

struct CameraManagerInitData: ManagerInitData{};

struct CarManagerInitData: ManagerInitData{
    btDynamicsWorld* dynamicsWorld;
    
    CarManagerInitData(btDynamicsWorld* dw) : dynamicsWorld(dw) {}
};

struct DrawManagerInitData: ManagerInitData{
    std::vector<GameObject*> gameObjects;
    LightsData lightsData;
    
    DrawManagerInitData(std::vector<GameObject*> gobjs, LightsData ld)
    : gameObjects(gobjs), lightsData(ld) {}
};

struct InputManagerInitData: ManagerInitData{
    GLFWwindow* window;
    
    InputManagerInitData(GLFWwindow* win) : window(win) {}
};

struct LightsManagerInitData: ManagerInitData{};

struct PhysicsManagerInitData: ManagerInitData{
    json sceneJson;
    
    PhysicsManagerInitData(json sj) : sceneJson(sj) {}
};

struct SceneManagerInitData: ManagerInitData{
    GLFWwindow* window;
    
    SceneManagerInitData(GLFWwindow* win) : window(win) {}
};

struct UIManagerInitData: ManagerInitData{
    BaseProject* baseProject;
    
    UIManagerInitData(BaseProject* bp) : baseProject(bp) {}
};

#endif
