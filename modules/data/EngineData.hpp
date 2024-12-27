#ifndef ENGINE_DATA_HPP
#define ENGINE_DATA_HPP

/*
 "World Data" is game data that either:
    - rarely change, unless special actions are performed (e.g., first/third person view change).
    - refer to engine-related parameters, so special attention should be paid.
 */

// VULKAN DATA
BaseProject* EngineBaseProject = nullptr;

// SCREEN DATA
GLFWwindow* EngineWindow = nullptr;
uint32_t EngineCurrentImage;
int EngineCurrentView;
int EngineCurrentScene;
float EngineAspectRatio = 4.0f/3.0f;

// SIMULATION DATA
float EngineDeltaTime = 60;

#endif
