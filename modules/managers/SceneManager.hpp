#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "Utils.hpp"
#include "../modules/engine/pattern/Receiver.hpp"
#include "../modules/data/WorldData.hpp"

class SceneManager : public Manager, public Receiver {
    
protected:
    
    void onQuit() {
        glfwSetWindowShouldClose(EngineWindow, GL_TRUE);
    }
    
public:
    
    void init() override {}
    
    void update() override {}
    
    void cleanup() override {}
    
    void onSignal(std::string id, std::any data) override {
        if (id == QUIT_SIGNAL) {
            onQuit();
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }
    
};

#endif
