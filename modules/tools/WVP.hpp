#ifndef WVP_HPP
#define WVP_HPP

#include "Types.hpp"
#include "Utils.hpp"

glm::mat4 MakeViewProjectionLookInDirection(glm::vec3 Pos, float Yaw, float Pitch, float Roll, float FOVy, float Ar, float nearPlane, float farPlane) {
    // Create a View Projection Matrix with the following characteristics:
    // Projection:
    //    - Perspective with:
    //    - Fov-y defined in formal parameter >FOVy<
    //  - Aspect ratio defined in formal parameter >Ar<
    //  - Near Plane distance defined in formal parameter >nearPlane<
    //  - Far Plane distance defined in formal parameter >farPlane<
    glm::mat4 Mprj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
    Mprj[1][1] *= -1;
    
    // View:
    //    - Use the Look-In-Direction model with:
    //    - Camera Positon defined in formal parameter >Pos<
    //    - Looking direction defined in formal parameter >Yaw<
    //    - Looking elevation defined in formal parameter >Pitch<
    //    - Looking rool defined in formal parameter >Roll<
    glm::mat4 Mv =
    glm::rotate(glm::mat4(1.0), -Roll, Z_AXIS) *
    glm::rotate(glm::mat4(1.0), -Pitch, X_AXIS) *
    glm::rotate(glm::mat4(1.0), -Yaw, Y_AXIS) *
    glm::translate(glm::mat4(1.0), -Pos);
    
    return Mprj * Mv;
}

glm::mat4 MakeViewProjectionLookAt(glm::vec3 Pos, glm::vec3 Target, glm::vec3 Up, float Roll, float FOVy, float Ar, float nearPlane, float farPlane) {
    // Create a View Projection Matrix with the following characteristics:
    
    // Projection:
    //    - Perspective with:
    //    - Fov-y defined in formal parameter >FOVy<
    //  - Aspect ratio defined in formal parameter >Ar<
    //  - Near Plane distance defined in formal parameter >nearPlane<
    //  - Far Plane distance defined in formal parameter >farPlane<
    glm::mat4 Mprj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
    Mprj[1][1] *= -1;
    
    // View:
    //    - Use the Look-At model with:
    //    - Camera Positon defined in formal parameter >Pos<
    //    - Camera Target defined in formal parameter >Target<
    //    - Up vector defined in formal parameter >Up<
    //    - Looking rool defined in formal parameter >Roll<
    glm::mat4 Mv =
    glm::rotate(glm::mat4(1.0), -Roll, Z_AXIS) *
    glm::lookAt(Pos, Target, Up);

    return Mprj * Mv;
}

glm::mat4 MakeWorld(glm::vec3 Pos, float Yaw, float Pitch, float Roll) {
    // Create a World Matrix with the following characteristics:
    //    - Object Positon defined in formal parameter >Pos<
    //    - Euler angle rotation yaw defined in formal parameter >Yaw<
    //    - Euler angle rotation pitch defined in formal parameter >Pitch<
    //    - Euler angle rotation roll defined in formal parameter >Roll<
    //  - Scaling constant and equal to 1 (and not passed to the procedure)
    glm::mat4 M =
    glm::translate(glm::mat4(1.0f), Pos) *
    glm::rotate(glm::mat4(1.0f), Yaw, Y_AXIS) *
    glm::rotate(glm::mat4(1.0f), Pitch, X_AXIS) *
    glm::rotate(glm::mat4(1.0f), Roll, Z_AXIS);

    return M;
}

// utility for lights
glm::mat4 getCarTextureWorldMatrix(CarWorldData carWorldData){
    float adjustedRoll = std::clamp(carWorldData.roll, -0.005f, 0.005f);
    return MakeWorld(carWorldData.position, carWorldData.yaw, carWorldData.pitch, adjustedRoll);
}

#endif
