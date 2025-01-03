#ifndef WORLD_DATA_HPP
#define WORLD_DATA_HPP

#include "../modules/engine/main/GameObject.hpp"

/*
 "World Data" is game data that frequently changes (e.g., due to user actions).
 */

// PHYSICS DATA
btDynamicsWorld* dynamicsWorld;
btRaycastVehicle* vehicle;

std::vector<btRigidBody*> addRigidBodyQueue = {};
std::vector<btRigidBody*> removeRigidBodyQueue = {};

// LIGHTS DATA
LightsData lightsData;

// AUDIO DATA
json audioData;

// INPUT DATA
glm::vec3 carMovementInput = ZERO_VEC3;
glm::vec3 cameraRotationInput = ZERO_VEC3;

// SCENE DATA
std::vector<GameObject*> gameObjects;
CameraWorldData cameraWorldData;
CarWorldData carWorldData;
glm::mat4 vehicleTextureWorldMatrix;

std::string nextCheckpointId;

#endif
