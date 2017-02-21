#include "Triangle.h"

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float transparency = 0.0f, float refractive_index = 1.0f)
	: v0(v0), v1(v1), v2(v2), color(color), transparency_(transparency), refractive_index_(refractive_index)
{
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 normal, float transparency = 0.0f, float refractive_index = 1.0f)
	: v0(v0), v1(v1), v2(v2), normal(normal), color(color), transparency_(transparency), refractive_index_(refractive_index)
{
}

void Triangle::ComputeNormal()
{
	glm::vec3 e1 = v1 - v0;
	glm::vec3 e2 = v2 - v0;
	normal = glm::normalize(glm::cross(e2, e1));
}