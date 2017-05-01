#include "Camera.h"

Camera::Camera(glm::vec3 position, float pitch, float yaw, float roll, float focal_length)
	: position(position), focal_length(focal_length)
{
	ComputeCameraTransform(pitch, yaw, roll);
}

void Camera::ComputeCameraTransform(float pitch, float yaw, float roll)
{
	worldToCamera = glm::mat4(); //builds identity matrix

	worldToCamera = glm::translate(worldToCamera, -1.0f * position);
	worldToCamera = glm::rotate(worldToCamera, glm::radians(360.0f - pitch), glm::vec3(0.0f, 1.0f, 0.0f));
	worldToCamera = glm::rotate(worldToCamera, glm::radians(360.0f - yaw), glm::vec3(1.0f, 0.0f, 0.0f));
	worldToCamera = glm::rotate(worldToCamera, glm::radians(360.0f - roll), glm::vec3(0.0f, 0.0f, 1.0f));
	worldToCamera = lookAt (position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Translate(glm::vec3 translation)
{
	//position = position + translation;
	worldToCamera = glm::translate(worldToCamera, 1.0f * translation);
}

void Camera::Rotate(float angle, glm::vec3 axis)
{
	worldToCamera = glm::rotate(worldToCamera, glm::radians(angle), axis);
}

glm::vec3 Camera::Position()
{
	return position;
}

