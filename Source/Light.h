#pragma once
#include <glm/detail/type_vec3.hpp>
#include "Camera.h"
#include <glm/detail/func_trigonometric.hpp>
#include <glm/gtx/rotate_vector.hpp>


struct Light {
	glm::vec3 position; // world space position of the light
	glm::vec3 color; // rgb light colour, 0.0f-1.0f
};

