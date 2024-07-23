#ifndef CAMERA_MANAGER_HPP
#define CAMERA_MANAGER_HPP

#include "tools/WVP.hpp"
#include "tools/Types.hpp"
#include "Utils.hpp"
#include "engine/Manager.hpp"
#include "engine/Observer.hpp"

struct CameraManager : public Observer, public Manager {
    
protected:
    
    int currentView;
    CameraData cameraData;
    glm::vec3 cameraPosition;
    glm::mat4 viewProjection;
    
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
    
    // CAMERA FUNCTIONS
    
    // These functions switch to a particular camera
    
    void switchToThirdPersonCamera(){
        cameraData.CamPitch = PITCH_THIRD;
        cameraData.CamYaw   = YAW_THIRD;
        cameraData.CamDist  = DIST_THIRD;
        cameraData.CamRoll  = ROLL_THIRD;
    }
    
    void switchToFirstPersonCamera(){
        cameraData.CamPitch = PITCH_FIRST;
        cameraData.CamYaw   = YAW_FIRST;
        cameraData.CamRoll  = ROLL_FIRST;
    }
    
    // These functions update the camera data
    
    void updateThirdPersonCamera(float pitch, float yaw, float roll, float aspectRatio, float deltaT, glm::vec3 cameraRotationInput, glm::vec3 carMovementInput, glm::vec3 carPosition){
        
        // moves camera
        cameraData.CamYaw += ROT_SPEED * deltaT * cameraRotationInput.y;
        cameraData.CamPitch -= ROT_SPEED * deltaT * cameraRotationInput.x;
        cameraData.CamRoll -= ROT_SPEED * deltaT * cameraRotationInput.z;
        cameraData.CamDist -= ROT_SPEED * deltaT * carMovementInput.y;
        
        // limits camera movement
        cameraData.CamYaw = (cameraData.CamYaw < MIN_YAW_THIRD ? MIN_YAW_THIRD : (cameraData.CamYaw > MAX_YAW_THIRD ? MAX_YAW_THIRD : cameraData.CamYaw));
        cameraData.CamPitch = (cameraData.CamPitch < MIN_PITCH_THIRD ? MIN_PITCH_THIRD : (cameraData.CamPitch > MAX_PITCH_THIRD ? MAX_PITCH_THIRD : cameraData.CamPitch));
        cameraData.CamRoll = (cameraData.CamRoll < MIN_ROLL_THIRD ? MIN_ROLL_THIRD : (cameraData.CamRoll > MAX_ROLL_THIRD ? MAX_ROLL_THIRD : cameraData.CamRoll));
        cameraData.CamDist = (cameraData.CamDist < MIN_DIST_THIRD ? MIN_DIST_THIRD : (cameraData.CamDist > MAX_DIST_THIRD ? MAX_DIST_THIRD : cameraData.CamDist));
        
        // computes camera position
        glm::vec3 camTarget = carPosition + glm::vec3(glm::rotate(ONE_MAT4, yaw, Y_AXIS) * glm::vec4(CamTargetDelta,1));
        
        glm::mat4 cameraTransform = glm::rotate(ONE_MAT4, yaw + cameraData.CamYaw, Y_AXIS)
        * glm::rotate(ONE_MAT4, -cameraData.CamPitch - pitch, X_AXIS)
        * glm::rotate(ONE_MAT4, roll + cameraData.CamRoll, Z_AXIS);
        
        cameraPosition = camTarget + glm::vec3(cameraTransform * glm::vec4(0, 0, cameraData.CamDist, 1));
        
        // builds third person view matrix
        viewProjection = MakeViewProjectionLookAt(cameraPosition, camTarget, Y_AXIS, cameraData.CamRoll, DEG_90, aspectRatio, NEAR_PLANE, FAR_PLANE);
    }
    
    void updateFirstPersonCamera(float pitch, float yaw, float roll, float aspectRatio, float deltaT, glm::vec3 cameraRotationInput, glm::vec3 carPosition){
        
        // moves camera
        cameraData.CamYaw -= ROT_SPEED * deltaT * cameraRotationInput.y;
        cameraData.CamPitch -= ROT_SPEED * deltaT * cameraRotationInput.x;
        cameraData.CamRoll -= ROT_SPEED * deltaT * cameraRotationInput.z;
        
        // limits camera movement
        cameraData.CamYaw = (cameraData.CamYaw < MIN_YAW_FIRST ? MIN_YAW_FIRST : (cameraData.CamYaw > MAX_YAW_FIRST ? MAX_YAW_FIRST : cameraData.CamYaw));
        cameraData.CamPitch = (cameraData.CamPitch < MIN_PITCH_FIRST ? MIN_PITCH_FIRST : (cameraData.CamPitch > MAX_PITCH_FIRST ? MAX_PITCH_FIRST : cameraData.CamPitch));
        cameraData.CamRoll = (cameraData.CamRoll < MIN_ROLL_FIRST ? MIN_ROLL_FIRST : (cameraData.CamRoll > MAX_ROLL_FIRST ? MAX_ROLL_FIRST : cameraData.CamRoll));
        
        float adjustedRoll = std::clamp(roll, -0.005f, 0.005f);
        
        glm::mat4 cameraTransform = glm::rotate(ONE_MAT4, yaw, Y_AXIS)
        * glm::rotate(ONE_MAT4, -pitch, X_AXIS)
        * glm::rotate(ONE_MAT4, adjustedRoll, Z_AXIS);
        
        cameraPosition = carPosition + glm::vec3(cameraTransform * glm::vec4(Cam1stPos,1));
        
        // builds first person view matrix
        viewProjection = MakeViewProjectionLookInDirection(cameraPosition, yaw + cameraData.CamYaw, cameraData.CamPitch, cameraData.CamRoll, DEG_90, aspectRatio, NEAR_PLANE, FAR_PLANE);
    }
    
public:
    
    void init(std::vector<void*> params) override {
        currentView = THIRD_PERSON_VIEW;
        switchToThirdPersonCamera();
    }
    
    void update(std::vector<void*> params) override {
        float pitch;
        float yaw;
        float roll;
        float aspectRatio;
        float deltaT;
        glm::vec3 cameraRotationInput;
        glm::vec3 carMovementInput;
        glm::vec3 carPosition;
        if (params.size() == 8) {
            pitch = *static_cast<float*>(params[0]);
            yaw = *static_cast<float*>(params[1]);
            roll = *static_cast<float*>(params[2]);
            aspectRatio = *static_cast<float*>(params[3]);
            deltaT = *static_cast<float*>(params[4]);
            cameraRotationInput = *static_cast<glm::vec3*>(params[5]);
            carMovementInput = *static_cast<glm::vec3*>(params[6]);
            carPosition = *static_cast<glm::vec3*>(params[7]);
        } else {
            std::cout << "CameraManager.update(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        if(currentView == THIRD_PERSON_VIEW){
            updateThirdPersonCamera(pitch, yaw, roll, aspectRatio, deltaT, cameraRotationInput, carMovementInput, carPosition);
        }
        else{
            updateFirstPersonCamera(pitch, yaw, roll, aspectRatio, deltaT, cameraRotationInput, carPosition);
        }
    }
    
    glm::vec3 getCameraPosition(){
        return cameraPosition;
    }
    
    glm::mat4 getViewProjection(){
        return viewProjection;
    }
    
    void onChangeView() override {
        if(currentView == THIRD_PERSON_VIEW){
            currentView = FIRST_PERSON_VIEW;
            switchToFirstPersonCamera();
        }
        else{
            currentView = THIRD_PERSON_VIEW;
            switchToThirdPersonCamera();
        }
    }
    
};

#endif
