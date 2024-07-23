#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#import "Utils.hpp"

struct SceneManager : public Observer, public Manager {
    
protected:
    
    GLFWwindow* window;
    int currentScene;
    bool debounce;
    int currentDebounce;
    
public:
    
    Subject shouldChangeView;
    
    void init(std::vector<void*> params) override {
        GLFWwindow* window = nullptr;
        if (params.size() == 1) {
            window = static_cast<GLFWwindow*>(params[0]);
        } else {
            std::cout << "SceneManager.init(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        this->window = window;
        
        currentScene = FIRST_PERSON_SCENE;
        debounce = false;
        currentDebounce = 0;
    }
    
    void addObservers(std::vector<Observer*> observers) {
        for (Observer* observer : observers) {
            shouldChangeView.addObserver(observer);
        }
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
