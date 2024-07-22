#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include "Camera.hpp"
#include "Utils.hpp"

void shouldQuit(GLFWwindow* window){
    // Standard procedure to quit when the ESC key is pressed
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

// returns true if should rebuild pipeline
bool shouldChangeScene(GLFWwindow* window, CameraData* cameraData, int* currScene, bool* debounce, int* curDebounce, glm::vec3* dampedCamPos, glm::vec3 Pos){
    if(glfwGetKey(window, GLFW_KEY_SPACE)) {
        if(!*debounce) {
            *debounce = true;
            *curDebounce = GLFW_KEY_SPACE;
            *currScene = (*currScene+1) % 2;
            if(*currScene == THIRD_PERSON_SCENE) {
                switchToThirdPersonCamera(cameraData);
                *dampedCamPos = Pos;
            } else if(*currScene == FIRST_PERSON_SCENE) {
                switchToFirstPersonCamera(cameraData);
            }
            std::cout << "Scene : " << *currScene << "\n";
            return true;
        }
    } else {
        if((*curDebounce == GLFW_KEY_SPACE) && *debounce) {
            *debounce = false;
            *curDebounce = 0;
        }
        return false;
    }
    return false;
}

#endif
