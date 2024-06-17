#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#ifndef UTILS_HPP
#define UTILS_HPP

#include <glm/glm.hpp>

// scene ids
const int THIRD_PERSON_SCENE = 0;
const int FIRST_PERSON_SCENE = 1;

// 3D axes
const glm::vec3 X_AXIS(1.0f, 0.0f, 0.0f);
const glm::vec3 Y_AXIS(0.0f, 1.0f, 0.0f);
const glm::vec3 Z_AXIS(0.0f, 0.0f, 1.0f);

#endif
