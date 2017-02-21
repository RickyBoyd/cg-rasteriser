#pragma once

#include <glm/glm.hpp>

class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 color;
	float transparency_;
	float refractive_index_;

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float transparency, float refractive_index);

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 normal, float transparency, float refractive_index);

	void ComputeNormal();
};