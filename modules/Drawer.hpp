#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "Scene.hpp"
#include "WVP.hpp"
#include "Utils.hpp"

// instance IDs declaration
std::vector<std::string> car = {"car"};
std::vector<std::string> coins = {};
std::vector<std::string> world = {
    "track", "barrier", "finish_line_top", "finish_line_floor", "small_ramps", "airship", "tower", "banners",
    "big_star", "rainbow"
};

void drawCar(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = car.begin(); it != car.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        ubo->mMat = MakeWorld(Pos, Yaw + deltaA[i], usePitch[i], 0) * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawWorld(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = world.begin(); it != world.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        ubo->mMat = scene->I[i].Wm * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void drawCoins(Scene* scene, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo, int currentImage, float Yaw, glm::vec3 Pos, glm::mat4 baseCar, glm::mat4 ViewPrj, glm::vec3 **deltaP, float *deltaA, float *usePitch){
    for (std::vector<std::string>::iterator it = coins.begin(); it != coins.end(); it++) {
        int i = scene->InstanceIds[it->c_str()];
        
        // updates coin's transform matrix
        scene->I[i].Wm = glm::rotate(scene->I[i].Wm, DEG_5, Z_AXIS);
        
        ubo->mMat = scene->I[i].Wm * baseCar;
        ubo->mvpMat = ViewPrj * ubo->mMat;
        ubo->nMat = glm::inverse(glm::transpose(ubo->mMat));

        scene->DS[i]->map(currentImage, ubo, sizeof(*ubo), 0);
        scene->DS[i]->map(currentImage, gubo, sizeof(*gubo), 2);
    }
}

void addInstanceToWorld(std::string instance_id){
    world.push_back(instance_id);
}

void addInstanceToCoins(std::string instance_id){
    coins.push_back(instance_id);
}

#endif
