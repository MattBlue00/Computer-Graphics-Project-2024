#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "managers/CameraManager.hpp"
#include "Utils.hpp"
#include "engine/Observer.hpp"
#include "engine/Subject.hpp"
#include "engine/Manager.hpp"

struct InputManager : public Manager {
    
protected:
    
    GLFWwindow* window;
    
    Subject shouldQuitSubject;
    Subject shouldChangeSceneSubject;
    Subject shouldUpdateDebounce;
    Subject shouldChangeHeadlightsStatus;
    
    void checkShouldQuit(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            shouldQuitSubject.notifyQuit();
        }
    }
    
    void checkShouldChangeScene(GLFWwindow* window, bool* shouldRebuildPipeline) {
        if (glfwGetKey(window, GLFW_KEY_SPACE)) {
            shouldChangeSceneSubject.notifyChangeScene();
            *shouldRebuildPipeline = true;
        }
        else{
            shouldUpdateDebounce.notifyUpdateDebounce();
            *shouldRebuildPipeline = false;
        }
    }
    
    void checkShouldChangeHeadlightsStatus(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_L)) {
            shouldChangeHeadlightsStatus.notifyChangeHeadlightsStatus();
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
    
    void addObserversForSceneEvents(std::vector<Observer*> observers) {
        for (Observer* observer : observers) {
            shouldQuitSubject.addObserver(observer);
            shouldChangeSceneSubject.addObserver(observer);
            shouldUpdateDebounce.addObserver(observer);
        }
    }
    
    void addObserversForLightEvents(std::vector<Observer*> observers) {
        for (Observer* observer : observers) {
            shouldChangeHeadlightsStatus.addObserver(observer);
        }
    }
    
    void update(std::vector<void*> params) override {
        bool* shouldRebuildPipeline = nullptr;
        if (params.size() == 1) {
            shouldRebuildPipeline = static_cast<bool*>(params[0]);
        } else {
            std::cout << "InputManager.update(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        checkShouldQuit(window);
        checkShouldChangeScene(window, shouldRebuildPipeline);
        checkShouldChangeHeadlightsStatus(window);
    }
    
};

#endif
