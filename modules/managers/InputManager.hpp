#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "managers/CameraManager.hpp"
#include "Utils.hpp"
#include "engine/main/Manager.hpp"
#include "../modules/data/Signals.hpp"
#include "../modules/data/SignalTypes.hpp"

class InputManager : public Manager {
    
protected:
    
    void checkShouldQuit() {
        if (glfwGetKey(EngineWindow, GLFW_KEY_ESCAPE)) {
            quitSignal.emit({});
        }
    }
    
    void checkShouldChangeScene() {
        if (glfwGetKey(EngineWindow, GLFW_KEY_SPACE)) {
            changeSceneSignal.emit({});
            rebuildPipelineSignal.emit({});
        }
        else{
            updateDebounceSignal.emit({});
        }
    }
    
    void checkShouldChangeHeadlightsStatus() {
        if ((glfwGetKey(EngineWindow, GLFW_KEY_L))) {
            headlightsChangeSignal.emit({});
        }
    }
    
    void checkResetView() {
        if (glfwGetKey(EngineWindow, GLFW_KEY_V)) {
            resetViewSignal.emit({});
        }
    }
    
public:
    
    void init() override {}
    
    void update() override {
        checkShouldQuit();
        checkShouldChangeScene();
        checkShouldChangeHeadlightsStatus();
        checkResetView();
    }
    
    void cleanup() override {}
    
};

#endif
