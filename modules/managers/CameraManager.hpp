#ifndef CAMERA_MANAGER_HPP
#define CAMERA_MANAGER_HPP

#include "tools/WVP.hpp"
#include "tools/Types.hpp"
#include "Utils.hpp"
#include "engine/main/Manager.hpp"
#include "engine/pattern/Receiver.hpp"
#include "../modules/data/EngineData.hpp"
#include "../modules/data/WorldData.hpp"
#include "../modules/data/SignalTypes.hpp"
#include <any>

class CameraManager : public Manager, public Receiver {
    
protected:
    
    // CAMERA CONSTANTS
    
    const float ROT_SPEED = DEG_120;
    const float NEAR_PLANE = 0.1f;
    const float FAR_PLANE = 2000.0f;
    
    // third person constants
    
    const float MIN_YAW_THIRD = 0.0f;
    const float MAX_YAW_THIRD = 2*M_PI;
    const float MIN_PITCH_THIRD = 0.0f;
    const float MAX_PITCH_THIRD = M_PI_2-0.01f;
    const float MIN_ROLL_THIRD = -M_PI;
    const float MAX_ROLL_THIRD = M_PI;
    const float MIN_DIST_THIRD = 2.0f;
    const float MAX_DIST_THIRD = 6.0f;
    
    const float PITCH_THIRD = DEG_5;
    const float YAW_THIRD = M_PI;
    const float DIST_THIRD = 4.0f;
    const float ROLL_THIRD = 0.0f;
    
    // first person constants
    
    const float MIN_YAW_FIRST = M_PI_2;
    const float MAX_YAW_FIRST = 1.5f*M_PI;
    const float MIN_PITCH_FIRST = -0.1f*M_PI;
    const float MAX_PITCH_FIRST = 0.1f*M_PI;
    const float MIN_ROLL_FIRST = -M_PI;
    const float MAX_ROLL_FIRST = M_PI;
    
    const float PITCH_FIRST = DEG_0;
    const float YAW_FIRST = M_PI;
    const float ROLL_FIRST = 0.0f;
    
    // other constants
    const int THIRD_PERSON_VIEW = 0;
    const int FIRST_PERSON_VIEW = 1;
    
    // Third person camera placement
    const glm::vec3 CamTargetDelta = glm::vec3(0.0f, 1.5f, 0.0f);
    
    // First person camera placement
    const glm::vec3 Cam1stPos = glm::vec3(0.0f, 2.0f, 0.3f);
    
    int waitChange = 60;
    
    // CAMERA FUNCTIONS
    
    // These functions switch to a particular camera
    
    void switchToThirdPersonCamera(){
        cameraWorldData.pitch       = PITCH_THIRD;
        cameraWorldData.yaw         = YAW_THIRD;
        cameraWorldData.distance    = DIST_THIRD;
        cameraWorldData.roll        = ROLL_THIRD;
    }
    
    void switchToFirstPersonCamera(){
        cameraWorldData.pitch = PITCH_FIRST;
        cameraWorldData.yaw   = YAW_FIRST;
        cameraWorldData.roll  = ROLL_FIRST;
    }
    
    // These functions update the camera data
    
    void updateThirdPersonCamera(float pitch, float yaw, float roll, glm::vec3 cameraRotationInput, glm::vec3 carMovementInput, glm::vec3 carPosition){
        
        // moves camera
        cameraWorldData.yaw         += ROT_SPEED * EngineDeltaTime * cameraRotationInput.y;
        cameraWorldData.pitch       -= ROT_SPEED * EngineDeltaTime * cameraRotationInput.x;
        cameraWorldData.roll        -= ROT_SPEED * EngineDeltaTime * cameraRotationInput.z;
        cameraWorldData.distance    -= ROT_SPEED * EngineDeltaTime * carMovementInput.y;
        
        // limits camera movement
        cameraWorldData.yaw = (cameraWorldData.yaw < MIN_YAW_THIRD ? MIN_YAW_THIRD : (cameraWorldData.yaw > MAX_YAW_THIRD ? MAX_YAW_THIRD : cameraWorldData.yaw));
        cameraWorldData.pitch = (cameraWorldData.pitch < MIN_PITCH_THIRD ? MIN_PITCH_THIRD : (cameraWorldData.pitch > MAX_PITCH_THIRD ? MAX_PITCH_THIRD : cameraWorldData.pitch));
        cameraWorldData.roll = (cameraWorldData.roll < MIN_ROLL_THIRD ? MIN_ROLL_THIRD : (cameraWorldData.roll > MAX_ROLL_THIRD ? MAX_ROLL_THIRD : cameraWorldData.roll));
        cameraWorldData.distance = (cameraWorldData.distance < MIN_DIST_THIRD ? MIN_DIST_THIRD : (cameraWorldData.distance > MAX_DIST_THIRD ? MAX_DIST_THIRD : cameraWorldData.distance));
        
        // computes camera position
        glm::vec3 camTarget = carPosition + glm::vec3(glm::rotate(ONE_MAT4, yaw, Y_AXIS) * glm::vec4(CamTargetDelta,1));
        
        glm::mat4 cameraTransform = glm::rotate(ONE_MAT4, yaw + cameraWorldData.yaw, Y_AXIS)
        * glm::rotate(ONE_MAT4, -cameraWorldData.pitch - pitch, X_AXIS)
        * glm::rotate(ONE_MAT4, roll + cameraWorldData.roll, Z_AXIS);
        
        cameraWorldData.position = camTarget + glm::vec3(cameraTransform * glm::vec4(0, 0, cameraWorldData.distance, 1));
        
        // builds third person view matrix
        cameraWorldData.viewProjection = MakeViewProjectionLookAt(cameraWorldData.position, camTarget, Y_AXIS, cameraWorldData.roll, DEG_90, EngineAspectRatio, NEAR_PLANE, FAR_PLANE);
    }
    
    void updateFirstPersonCamera(float pitch, float yaw, float roll, glm::vec3 cameraRotationInput, glm::vec3 carPosition){
        
        // moves camera
        cameraWorldData.yaw      -= ROT_SPEED * EngineDeltaTime * cameraRotationInput.y;
        cameraWorldData.pitch    -= ROT_SPEED * EngineDeltaTime * cameraRotationInput.x;
        cameraWorldData.roll     -= ROT_SPEED * EngineDeltaTime * cameraRotationInput.z;
        
        // limits camera movement
        cameraWorldData.yaw = (cameraWorldData.yaw < MIN_YAW_FIRST ? MIN_YAW_FIRST : (cameraWorldData.yaw > MAX_YAW_FIRST ? MAX_YAW_FIRST : cameraWorldData.yaw));
        cameraWorldData.pitch = (cameraWorldData.pitch < MIN_PITCH_FIRST ? MIN_PITCH_FIRST : (cameraWorldData.pitch > MAX_PITCH_FIRST ? MAX_PITCH_FIRST : cameraWorldData.pitch));
        cameraWorldData.roll = (cameraWorldData.roll < MIN_ROLL_FIRST ? MIN_ROLL_FIRST : (cameraWorldData.roll > MAX_ROLL_FIRST ? MAX_ROLL_FIRST : cameraWorldData.roll));
        
        float adjustedRoll = std::clamp(roll, -0.005f, 0.005f);
        
        glm::mat4 cameraTransform = glm::rotate(ONE_MAT4, yaw, Y_AXIS)
        * glm::rotate(ONE_MAT4, -pitch, X_AXIS)
        * glm::rotate(ONE_MAT4, adjustedRoll, Z_AXIS);
        
        cameraWorldData.position = carPosition + glm::vec3(cameraTransform * glm::vec4(Cam1stPos,1));
        
        // builds first person view matrix
        cameraWorldData.viewProjection = MakeViewProjectionLookInDirection(cameraWorldData.position, yaw + cameraWorldData.yaw, cameraWorldData.pitch, cameraWorldData.roll, DEG_90, EngineAspectRatio, NEAR_PLANE, FAR_PLANE);
    }
    
    void onChangeView() {
        if(EngineCurrentView == THIRD_PERSON_VIEW){
            EngineCurrentView = FIRST_PERSON_VIEW;
            switchToFirstPersonCamera();
        }
        else{
            EngineCurrentView = THIRD_PERSON_VIEW;
            switchToThirdPersonCamera();
        }
    }
    
    void onResetView() {
        if(waitChange >= 60){
            if(EngineCurrentView == FIRST_PERSON_VIEW){
                switchToFirstPersonCamera();
            }
            else{
                switchToThirdPersonCamera();
            }
            
            waitChange = 0;
        }
    }
    
public:
    
    void init() override {
        EngineCurrentView = THIRD_PERSON_VIEW;
        switchToThirdPersonCamera();
    }
    
    void update() override {
        if(EngineCurrentView == THIRD_PERSON_VIEW){
            updateThirdPersonCamera(carWorldData.pitch, carWorldData.yaw, carWorldData.roll, cameraRotationInput, carMovementInput, carWorldData.position);
        }
        else{
            updateFirstPersonCamera(carWorldData.pitch, carWorldData.yaw, carWorldData.roll, cameraRotationInput, carWorldData.position);
        }
        
        if(waitChange < 60){
            waitChange++;
        }
    }
    
    void cleanup() override {}
    
    void handleData(std::string id, std::any data) override {
        if (id == CHANGE_VIEW_SIGNAL) {
            onChangeView();
        } else if (id == RESET_VIEW_SIGNAL) {
            onResetView();
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }
    
};

#endif
