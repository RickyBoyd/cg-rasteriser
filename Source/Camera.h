#pragma once
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_mat.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

class Camera
{
public:
	glm::mat4 worldToCamera;
	float focal_length;

	Camera(glm::vec3 position, float pitch, float yaw, float roll, float focal_length);

	//Getter for camera position
	glm::vec3 Position() const;

	void ComputeCameraTransform(float pitch, float yaw, float roll);

	//methods to translate and rotate camera
	void Translate(glm::vec3 translation);
	void Rotate(float angle, glm::vec3 axis);

private:
	glm::vec3 position;

};