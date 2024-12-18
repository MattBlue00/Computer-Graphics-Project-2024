#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "managers/CameraManager.hpp"
#include "Utils.hpp"
#include "engine/pattern/Observer.hpp"
#include "engine/pattern/Subject.hpp"
#include "engine/main/Manager.hpp"
#include "engine/main/utils/ManagerInitData.hpp"
#include "utils/ManagerInitData.hpp"
#include "utils/ManagerUpdateData.hpp"

Subject shouldQuitSubject;
Subject shouldChangeSceneSubject;
Subject shouldUpdateDebounce;
Subject shouldChangeHeadlightsStatus;

Subject resetViewSubject;

struct InputManager : public Manager {
    
protected:
    
    GLFWwindow* window;
    
    int waitHeadlights = 60;
    
    void checkShouldQuit(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            shouldQuitSubject.notifyQuit();
        }
    }
    
    void checkShouldChangeScene(GLFWwindow* window, bool* shouldRebuildPipeline, bool* sceneChanged) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) || *sceneChanged) {
            shouldChangeSceneSubject.notifyChangeScene();
            *shouldRebuildPipeline = true;
        }
        else{
            shouldUpdateDebounce.notifyUpdateDebounce();
            *shouldRebuildPipeline = false;
        }
    }
    
    void checkShouldChangeHeadlightsStatus(GLFWwindow* window, bool* headlightsChanged) {
        if ((glfwGetKey(window, GLFW_KEY_L) || *headlightsChanged) && waitHeadlights >= 60) {
            shouldChangeHeadlightsStatus.notifyChangeHeadlightsStatus();
            waitHeadlights = 0;
        }
        else{
            if(glfwGetKey(window, GLFW_KEY_L) || *headlightsChanged){
                waitHeadlights++;
            }
            else{
                waitHeadlights = 60;
            }
        }
    }
    
    void checkResetView(GLFWwindow* window, bool* viewReset) {
        if (glfwGetKey(window, GLFW_KEY_V) || *viewReset) {
            resetViewSubject.notifyResetView();
        }
    }
    
public:
    
    void init(ManagerInitData* param) override {
        auto* data = dynamic_cast<InputManagerInitData*>(param);
        
        if (!data) {
            throw std::runtime_error("Invalid type for ManagerInitData");
        }
        
        this->window = data->window;
    }
    
    void update(ManagerUpdateData* param) override {
        auto* data = dynamic_cast<InputManagerUpdateData*>(param);
        
        if (!data) {
            throw std::runtime_error("Invalid type for ManagerUpdateData");
        }
        
        checkShouldQuit(window);
        checkShouldChangeScene(window, data->shouldRebuildPipeline, data->sceneChanged);
        checkShouldChangeHeadlightsStatus(window, data->headlightsChanged);
        checkResetView(window, data->viewReset);
    }
    
    void cleanup() override {}
    
};

#endif
