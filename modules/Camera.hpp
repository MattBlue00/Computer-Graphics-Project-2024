#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Utils.hpp"

// camera data valid for any view
struct CameraData {
    float CamPitch;
    float CamYaw;
    float CamDist;
    float CamRoll;
};

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

void updateThirdPersonCamera(CameraData* cameraData, float rot_speed, float deltaT, glm::vec3 r, glm::vec3 m){
    cameraData->CamYaw += rot_speed * deltaT * r.y;
    cameraData->CamPitch -= rot_speed * deltaT * r.x;
    cameraData->CamRoll -= rot_speed * deltaT * r.z;
    cameraData->CamDist -= rot_speed * deltaT * m.y;

    cameraData->CamYaw = (cameraData->CamYaw < 0.0f ? 0.0f : (cameraData->CamYaw > 2*M_PI ? 2*M_PI : cameraData->CamYaw));
    cameraData->CamPitch = (cameraData->CamPitch < 0.0f ? 0.0f : (cameraData->CamPitch > M_PI_2-0.01f ? M_PI_2-0.01f : cameraData->CamPitch));
    cameraData->CamRoll = (cameraData->CamRoll < -M_PI ? -M_PI : (cameraData->CamRoll > M_PI ? M_PI : cameraData->CamRoll));
    cameraData->CamDist = (cameraData->CamDist < 7.0f ? 7.0f : (cameraData->CamDist > 15.0f ? 15.0f : cameraData->CamDist));
}

void updateFirstPersonCamera(CameraData* cameraData, float rot_speed, float deltaT, glm::vec3 r, glm::vec3 m){
    cameraData->CamYaw -= rot_speed * deltaT * r.y;
    cameraData->CamPitch -= rot_speed * deltaT * r.x;
    cameraData->CamRoll -= rot_speed * deltaT * r.z;

    cameraData->CamYaw = (cameraData->CamYaw < M_PI_2 ? M_PI_2 : (cameraData->CamYaw > 1.5*M_PI ? 1.5*M_PI : cameraData->CamYaw));
    cameraData->CamPitch = (cameraData->CamPitch < -0.25*M_PI ? -0.25*M_PI : (cameraData->CamPitch > 0.25*M_PI ? 0.25*M_PI : cameraData->CamPitch));
    cameraData->CamRoll = (cameraData->CamRoll < -M_PI ? -M_PI : (cameraData->CamRoll > M_PI ? M_PI : cameraData->CamRoll));
}

#endif
