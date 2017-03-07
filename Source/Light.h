#pragma once
#include <glm/detail/type_vec3.hpp>
#include "Camera.h"
#include <glm/detail/func_trigonometric.hpp>
#include <glm/gtx/rotate_vector.hpp>


struct Light {
	glm::vec3 position; // world space position of the light
	glm::vec3 color; // rgb light colour, 0.0f-1.0f

	glm::vec3 camera_position;

	void UpdateCameraSpacePosition(const Camera& camera)
	{
		glm::vec3 camera_space_position = position - camera.position;
		camera_space_position = glm::rotate(camera_space_position, glm::radians(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		camera_space_position = glm::rotate(camera_space_position, glm::radians(camera.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		camera_space_position = glm::rotate(camera_space_position, glm::radians(camera.roll), glm::vec3(0.0f, 0.0f, 1.0f));
		camera_position = camera_space_position;
	}
};

