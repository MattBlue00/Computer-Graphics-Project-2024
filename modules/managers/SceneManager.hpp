#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "Utils.hpp"
#include "../modules/engine/pattern/Receiver.hpp"
#include "../modules/data/WorldData.hpp"

class SceneManager : public Manager, public Receiver {
    
protected:
    
    bool debounce;
    int currentDebounce;
    
    int waitChange = 60;
    
    void onQuit() {
        glfwSetWindowShouldClose(EngineWindow, GL_TRUE);
    }
    
    void onChangeScene() {
        if(waitChange >= 60){
            if(!debounce) {
                debounce = true;
                currentDebounce = GLFW_KEY_SPACE;
                EngineCurrentScene = (EngineCurrentScene+1) % 2;
                if(EngineCurrentScene == THIRD_PERSON_SCENE) {
                    changeViewSignal.emit({});
                } else if(EngineCurrentScene == FIRST_PERSON_SCENE) {
                    changeViewSignal.emit({});
                }
                std::cout << "Camera : " << (EngineCurrentScene == 0 ? "Third Person" : "First Person") << "\n";
            }
            
            waitChange = 0;
        }
    }
    
    void onUpdateDebounce() {
        if((currentDebounce == GLFW_KEY_SPACE) && debounce) {
            debounce = false;
            currentDebounce = 0;
        }
    }
    
public:
    
    void init() override {
        EngineCurrentScene = THIRD_PERSON_SCENE;
        debounce = false;
        currentDebounce = 0;
    }
    
    void update() override {
        if(waitChange < 60){
            waitChange++;
        }
    }
    
    void cleanup() override {}
    
    void handleData(std::string id, std::any data) override {
        if (id == QUIT_SIGNAL) {
            onQuit();
        } else if (id == CHANGE_SCENE_SIGNAL) {
            onChangeScene();
        } else if (id == UPDATE_DEBOUNCE_SIGNAL) {
            onUpdateDebounce();
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }
    
};

#endif
