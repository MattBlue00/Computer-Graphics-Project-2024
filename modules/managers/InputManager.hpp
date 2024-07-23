#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "managers/CameraManager.hpp"
#include "Utils.hpp"
#include "engine/Observer.hpp"
#include "engine/Subject.hpp"
#include "engine/Manager.hpp"

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
    
    void init(std::vector<void*> params) override {
        GLFWwindow* window = nullptr;
        if (params.size() == 1) {
            window = static_cast<GLFWwindow*>(params[0]);
        } else {
            std::cout << "InputManager.init(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        this->window = window;
    }
    
    void update(std::vector<void*> params) override {
        bool* shouldRebuildPipeline = nullptr;
        bool* headlightsChanged = nullptr;
        bool* sceneChanged = nullptr;
        bool* viewReset = nullptr;
        if (params.size() == 4) {
            shouldRebuildPipeline = static_cast<bool*>(params[0]);
            headlightsChanged = static_cast<bool*>(params[1]);
            sceneChanged = static_cast<bool*>(params[2]);
            viewReset = static_cast<bool*>(params[3]);
        } else {
            std::cout << "InputManager.update(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        checkShouldQuit(window);
        checkShouldChangeScene(window, shouldRebuildPipeline, sceneChanged);
        checkShouldChangeHeadlightsStatus(window, headlightsChanged);
        checkResetView(window, viewReset);
    }
    
};

#endif
