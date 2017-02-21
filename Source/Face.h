#pragma once
#include "Material.h"
#include "Triangle.h"

class Face
{
public:
	std::vector<glm::vec3> vertices_;
	std::shared_ptr<std::vector<glm::vec2>> texture_coords_;
	std::shared_ptr<glm::vec3> normal_;
	Material material_;

	Face(std::vector<glm::vec3> vertices, std::shared_ptr<std::vector<glm::vec2>> texture_coords, std::shared_ptr<glm::vec3> normal, Material material);
	Face(const Face& face);
	~Face();

	Triangle ToTriangle(glm::vec3 transform, glm::vec3 scale);
};

