#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "Utils.hpp"
#include "utils/ManagerInitData.hpp"
#include "utils/ManagerUpdateData.hpp"

Subject shouldChangeView;

struct SceneManager : public Observer, public Manager {
    
protected:
    
    GLFWwindow* window;
    int currentScene;
    bool debounce;
    int currentDebounce;
    
public:
    
    void init(ManagerInitData* param) override {
        auto* data = dynamic_cast<SceneManagerInitData*>(param);
        
        if (!data) {
            throw std::runtime_error("Invalid type for ManagerInitData");
        }
        
        this->window = data->window;
        
        currentScene = THIRD_PERSON_SCENE;
        debounce = false;
        currentDebounce = 0;
    }
    
    void update(ManagerUpdateData* param) override {}
    
    void cleanup() override {}
    
    int getCurrentScene(){
        return currentScene;
    }
    
    // OBSERVER METHODS
    
    void onQuit() override {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    void onChangeScene() override {
        if(!debounce) {
            debounce = true;
            currentDebounce = GLFW_KEY_SPACE;
            currentScene = (currentScene+1) % 2;
            if(currentScene == THIRD_PERSON_SCENE) {
                shouldChangeView.notifyChangeView();
            } else if(currentScene == FIRST_PERSON_SCENE) {
                shouldChangeView.notifyChangeView();
            }
            std::cout << "Scene : " << currentScene << "\n";
        }
    }
    
    void onUpdateDebounce() override {
        if((currentDebounce == GLFW_KEY_SPACE) && debounce) {
            debounce = false;
            currentDebounce = 0;
        }
    }
    
};

#endif
