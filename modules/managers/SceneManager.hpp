#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#import "Utils.hpp"

struct SceneManager : public GameManager {
    
public:

    GLFWwindow* window;
    int currentScene;
    bool debounce;
    int currentDebounce;
    
    void init(GLFWwindow* window) override {
        this->window = window;
        currentScene = FIRST_PERSON_SCENE;
        debounce = false;
        currentDebounce = 0;
    }
    
    void update() override {
        
    }
    
    void cleanup() override {
        
    }
    
}
    
}

#endif
