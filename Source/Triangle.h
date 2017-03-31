#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class Triangle
{
public:
	glm::vec4 v0_;
	glm::vec4 v1_;
	glm::vec4 v2_;
	glm::vec3 normal;
	glm::vec3 color;
	float transparency_;
	float refractive_index_;


	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float transparency, float refractive_index);

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 normal, float transparency, float refractive_index);

	Triangle(glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, glm::vec3 color, float transparency, float refractive_index);

	// Member functions definitions including constructor
	Triangle(void);

	void ComputeNormal();

	std::vector<glm::vec4> Vertices();
};