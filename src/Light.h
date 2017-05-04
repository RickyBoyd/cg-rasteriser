#pragma once
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>
#include "Camera.h"
#include <glm/detail/func_trigonometric.hpp>
#include <glm/gtx/rotate_vector.hpp>


struct Light {
	glm::vec3 position; // world space position of the light
	glm::vec3 color; // rgb light colour, 0.0f-1.0f

	glm::vec3 camera_position;

	void UpdateCameraSpacePosition(const Camera& camera)
	{
		glm::vec4 light_pos = glm::vec4(position, 1.0f);
		light_pos =  camera.worldToCamera * light_pos;
		camera_position = glm::vec3(light_pos);
	}
};

