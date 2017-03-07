#pragma once
#include <glm/detail/type_vec3.hpp>

struct Camera
{
	glm::vec3 position; // world space position of the camera
	float pitch;
	float yaw;
	float roll;
	float focal_length;
};

