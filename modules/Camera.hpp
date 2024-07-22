#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "WVP.hpp"
#include "Utils.hpp"

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

// Third person camera placement
const glm::vec3 CamTargetDelta = glm::vec3(0.0f, 1.5f, 0.0f);

// First person camera placement
const glm::vec3 Cam1stPos = glm::vec3(0.0f, 2.0f, 0.3f);

// CAMERA DATA STRUCTURES

// camera data valid for any view
struct CameraData {
    float CamPitch;
    float CamYaw;
    float CamDist;
    float CamRoll;
};

// CAMERA FUNCTIONS

// These functions switch to a particular camera

void switchToThirdPersonCamera(CameraData* cameraData){
    cameraData->CamPitch = PITCH_THIRD;
    cameraData->CamYaw   = YAW_THIRD;
    cameraData->CamDist  = DIST_THIRD;
    cameraData->CamRoll  = ROLL_THIRD;
}

void switchToFirstPersonCamera(CameraData* cameraData){
    cameraData->CamPitch = PITCH_FIRST;
    cameraData->CamYaw   = YAW_FIRST;
    cameraData->CamRoll  = ROLL_FIRST;
}

// These functions update the camera data

void updateThirdPersonCamera(CameraData* cameraData, glm::vec3* CamPos, glm::vec3* dampedCamPos, glm::mat4* M, float Yaw, float Pitch, float Roll, float AspectRatio, float rot_speed, float deltaT, glm::vec3 cameraRotationInput, glm::vec3 carMovementInput, glm::vec3 Pos){
    
    // moves camera
    cameraData->CamYaw += rot_speed * deltaT * cameraRotationInput.y;
    cameraData->CamPitch -= rot_speed * deltaT * cameraRotationInput.x;
    cameraData->CamRoll -= rot_speed * deltaT * cameraRotationInput.z;
    cameraData->CamDist -= rot_speed * deltaT * carMovementInput.y;

    // limits camera movement
    cameraData->CamYaw = (cameraData->CamYaw < MIN_YAW_THIRD ? MIN_YAW_THIRD : (cameraData->CamYaw > MAX_YAW_THIRD ? MAX_YAW_THIRD : cameraData->CamYaw));
    cameraData->CamPitch = (cameraData->CamPitch < MIN_PITCH_THIRD ? MIN_PITCH_THIRD : (cameraData->CamPitch > MAX_PITCH_THIRD ? MAX_PITCH_THIRD : cameraData->CamPitch));
    cameraData->CamRoll = (cameraData->CamRoll < MIN_ROLL_THIRD ? MIN_ROLL_THIRD : (cameraData->CamRoll > MAX_ROLL_THIRD ? MAX_ROLL_THIRD : cameraData->CamRoll));
    cameraData->CamDist = (cameraData->CamDist < MIN_DIST_THIRD ? MIN_DIST_THIRD : (cameraData->CamDist > MAX_DIST_THIRD ? MAX_DIST_THIRD : cameraData->CamDist));
    
    // computes camera position
    glm::vec3 CamTarget = Pos + glm::vec3(glm::rotate(ONE_MAT4, Yaw, Y_AXIS) * glm::vec4(CamTargetDelta,1));
    
    glm::mat4 cameraTransform = glm::rotate(ONE_MAT4, Yaw + cameraData->CamYaw, Y_AXIS)
                                * glm::rotate(ONE_MAT4, -cameraData->CamPitch - Pitch, X_AXIS)
                                * glm::rotate(ONE_MAT4, Roll + cameraData->CamRoll, Z_AXIS);

    *CamPos = CamTarget + glm::vec3(cameraTransform * glm::vec4(0, 0, cameraData->CamDist, 1));
    *dampedCamPos = *CamPos;
    
    // builds third person view matrix
    *M = MakeViewProjectionLookAt(*dampedCamPos, CamTarget, Y_AXIS, cameraData->CamRoll, DEG_90, AspectRatio, NEAR_PLANE, FAR_PLANE);
}

void updateFirstPersonCamera(CameraData* cameraData, glm::mat4* M, float Yaw, float Pitch, float Roll, float AspectRatio, float rot_speed, float deltaT, glm::vec3 r, glm::vec3 m, glm::vec3 Pos){
    
    // moves camera
    cameraData->CamYaw -= rot_speed * deltaT * r.y;
    cameraData->CamPitch -= rot_speed * deltaT * r.x;
    cameraData->CamRoll -= rot_speed * deltaT * r.z;

    // limits camera movement
    cameraData->CamYaw = (cameraData->CamYaw < MIN_YAW_FIRST ? MIN_YAW_FIRST : (cameraData->CamYaw > MAX_YAW_FIRST ? MAX_YAW_FIRST : cameraData->CamYaw));
    cameraData->CamPitch = (cameraData->CamPitch < MIN_PITCH_FIRST ? MIN_PITCH_FIRST : (cameraData->CamPitch > MAX_PITCH_FIRST ? MAX_PITCH_FIRST : cameraData->CamPitch));
    cameraData->CamRoll = (cameraData->CamRoll < MIN_ROLL_FIRST ? MIN_ROLL_FIRST : (cameraData->CamRoll > MAX_ROLL_FIRST ? MAX_ROLL_FIRST : cameraData->CamRoll));
    
    float adjustedRoll = std::clamp(Roll, -0.01f, 0.01f);
    
    glm::mat4 cameraTransform = glm::rotate(ONE_MAT4, Yaw, Y_AXIS)
                                * glm::rotate(ONE_MAT4, -Pitch, X_AXIS)
                                * glm::rotate(ONE_MAT4, adjustedRoll, Z_AXIS);

    glm::vec3 Cam1Pos = Pos + glm::vec3(cameraTransform * glm::vec4(Cam1stPos,1));

    // builds first person view matrix
    *M = MakeViewProjectionLookInDirection(Cam1Pos, Yaw + cameraData->CamYaw, cameraData->CamPitch, cameraData->CamRoll, DEG_90, AspectRatio, NEAR_PLANE, FAR_PLANE);
}

#endif
