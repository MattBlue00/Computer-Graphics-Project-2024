#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "WVP.hpp"
#include "Utils.hpp"

// camera data valid for any view
struct CameraData {
    float CamPitch;
    float CamYaw;
    float CamDist;
    float CamRoll;
};

// Third person camera parameter
const glm::vec3 CamTargetDelta = glm::vec3(0,2,0);

// First person camera parameter
const glm::vec3 Cam1stPos = glm::vec3(0.49061f, 2.07f, 2.7445f);

// These functions switch to a particular camera

void switchToThirdPersonCamera(CameraData* cameraData){
    cameraData->CamPitch = glm::radians(20.0f);
    cameraData->CamYaw   = M_PI;
    cameraData->CamDist  = 10.0f;
    cameraData->CamRoll  = 0.0f;
}

void switchToFirstPersonCamera(CameraData* cameraData){
    cameraData->CamPitch = glm::radians(0.0f);
    cameraData->CamYaw   = M_PI;
    cameraData->CamDist  = 10.0f;
    cameraData->CamRoll  = 0.0f;
}

// These functions update the camera data

void updateThirdPersonCamera(CameraData* cameraData, glm::vec3* CamPos, glm::vec3* dampedCamPos, glm::mat4* M, float Yaw, float AspectRatio, float rot_speed, float deltaT, glm::vec3 r, glm::vec3 m, glm::vec3 Pos){
    
    // moves camera
    cameraData->CamYaw += rot_speed * deltaT * r.y;
    cameraData->CamPitch -= rot_speed * deltaT * r.x;
    cameraData->CamRoll -= rot_speed * deltaT * r.z;
    cameraData->CamDist -= rot_speed * deltaT * m.y;

    // limits camera movement
    cameraData->CamYaw = (cameraData->CamYaw < 0.0f ? 0.0f : (cameraData->CamYaw > 2*M_PI ? 2*M_PI : cameraData->CamYaw));
    cameraData->CamPitch = (cameraData->CamPitch < 0.0f ? 0.0f : (cameraData->CamPitch > M_PI_2-0.01f ? M_PI_2-0.01f : cameraData->CamPitch));
    cameraData->CamRoll = (cameraData->CamRoll < -M_PI ? -M_PI : (cameraData->CamRoll > M_PI ? M_PI : cameraData->CamRoll));
    cameraData->CamDist = (cameraData->CamDist < 7.0f ? 7.0f : (cameraData->CamDist > 15.0f ? 15.0f : cameraData->CamDist));
    
    // computes camera position
    glm::vec3 CamTarget = Pos + glm::vec3(glm::rotate(glm::mat4(1), Yaw, Y_AXIS) *
                     glm::vec4(CamTargetDelta,1));
    *CamPos = CamTarget + glm::vec3(glm::rotate(glm::mat4(1), Yaw + cameraData->CamYaw, Y_AXIS) * glm::rotate(glm::mat4(1), -cameraData->CamPitch, X_AXIS) *
                                   glm::vec4(0,0,cameraData->CamDist,1));

    // smoothes camera movement
    const float lambdaCam = 10.0f;
    *dampedCamPos = *CamPos * (1 - exp(-lambdaCam * deltaT)) +
        *dampedCamPos * exp(-lambdaCam * deltaT);
    
    // builds third person view matrix
    *M = MakeViewProjectionLookAt(*dampedCamPos, CamTarget, Y_AXIS, cameraData->CamRoll, DEG_90, AspectRatio, 0.1f, 500.0f);
}

void updateFirstPersonCamera(CameraData* cameraData, glm::mat4* M, float Yaw, float AspectRatio, float rot_speed, float deltaT, glm::vec3 r, glm::vec3 m, glm::vec3 Pos){
    
    // moves camera
    cameraData->CamYaw -= rot_speed * deltaT * r.y;
    cameraData->CamPitch -= rot_speed * deltaT * r.x;
    cameraData->CamRoll -= rot_speed * deltaT * r.z;

    // limits camera movement
    cameraData->CamYaw = (cameraData->CamYaw < M_PI_2 ? M_PI_2 : (cameraData->CamYaw > 1.5*M_PI ? 1.5*M_PI : cameraData->CamYaw));
    cameraData->CamPitch = (cameraData->CamPitch < -0.25*M_PI ? -0.25*M_PI : (cameraData->CamPitch > 0.25*M_PI ? 0.25*M_PI : cameraData->CamPitch));
    cameraData->CamRoll = (cameraData->CamRoll < -M_PI ? -M_PI : (cameraData->CamRoll > M_PI ? M_PI : cameraData->CamRoll));
    
    // computes camera position
    glm::vec3 Cam1Pos = Pos + glm::vec3(glm::rotate(glm::mat4(1), Yaw, Y_AXIS) *
                     glm::vec4(Cam1stPos,1));
    
    // builds first person view matrix
    *M = MakeViewProjectionLookInDirection(Cam1Pos, Yaw + cameraData->CamYaw, cameraData->CamPitch, cameraData->CamRoll, glm::radians(90.0f), AspectRatio, 0.1f, 500.0f);
}

#endif
