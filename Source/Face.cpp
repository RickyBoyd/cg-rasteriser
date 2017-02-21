#include "Face.h"



Face::Face(std::vector<glm::vec3> vertices, std::shared_ptr<std::vector<glm::vec2>> texture_coords, std::shared_ptr<glm::vec3> normal, Material material)
	: vertices_(vertices), texture_coords_(texture_coords), normal_(normal), material_(material)
{
}

Face::Face(const Face& face) 
	: vertices_(face.vertices_), texture_coords_(face.texture_coords_), normal_(face.normal_), material_(face.material_)
{
}


Face::~Face()
{
}

Triangle Face::ToTriangle(glm::vec3 transform, glm::vec3 scale)
{
	if (normal_ != nullptr) {
		return Triangle(
			vertices_[0] * scale + transform,
			vertices_[1] * scale + transform,
			vertices_[2] * scale + transform,
			material_.ambient_colour_,
			*normal_, 
			material_.transparency_,
			material_.refractive_index_);
	}
	else
	{
		return Triangle(
			vertices_[0] * scale + transform,
			vertices_[0] * scale + transform,
			vertices_[0] * scale + transform,
			material_.ambient_colour_,
			material_.transparency_,
			material_.refractive_index_);
	}
}
