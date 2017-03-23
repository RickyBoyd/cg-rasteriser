#include "Triangle.h"

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float transparency = 0.0f, float refractive_index = 1.0f)
	: color(color), transparency_(transparency), refractive_index_(refractive_index)
{
	v0_ = glm::vec4(v0, 1.0f);
	v1_ = glm::vec4(v1, 1.0f);
	v2_ = glm::vec4(v2, 1.0f);
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 normal, float transparency = 0.0f, float refractive_index = 1.0f)
	: normal(normal), color(color), transparency_(transparency), refractive_index_(refractive_index)
{
	v0_ = glm::vec4(v0, 1.0f);
	v1_ = glm::vec4(v1, 1.0f);
	v2_ = glm::vec4(v2, 1.0f);
}

Triangle::Triangle(glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, glm::vec3 color, float transparency = 0.0f, float refractive_index = 1.0f)
	: v0_(v0), v1_(v1), v2_(v2), color(color), transparency_(transparency), refractive_index_(refractive_index)
{
	ComputeNormal();
}

// Member functions definitions including constructor
Triangle::Triangle(void) {
   std::cout << "Triangle is being created" << std::endl;
}

void Triangle::ComputeNormal()
{
	glm::vec3 e1 = glm::vec3(v1_) - glm::vec3(v0_);
	glm::vec3 e2 = glm::vec3(v2_) - glm::vec3(v0_);
	normal = glm::normalize(glm::cross(e2, e1));
}