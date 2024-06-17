#ifndef CAMERA_HPP
#define CAMERA_HPP

struct CameraData {
    float CamPitch;
    float CamYaw;
    float CamDist;
    float CamRoll;
    glm::vec3 dampedCamPos;
};

#endif
