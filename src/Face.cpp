#include "Face.h"



Face::Face(std::vector<glm::vec3> vertices, std::shared_ptr<std::vector<glm::vec2>> texture_coords, std::shared_ptr<glm::vec3> normal, std::shared_ptr<Material> material)
	: vertices_(vertices), texture_coords_(texture_coords), normal_(normal), material_(material)
{
}

Face::Face(const Face& face) 
	: vertices_(face.vertices_), texture_coords_(face.texture_coords_), normal_(face.normal_), material_(face.material_)
{
}

Face::Face(std::vector<glm::vec3> vertices, std::shared_ptr<Material> material)
	: vertices_(vertices), material_(material)
{
}

Face::~Face()
{
}

Triangle Face::ToTriangle(glm::vec3 transform, glm::vec3 scale)
{
	if (normal_ != nullptr) 
	{
		if (texture_coords_->size() == 3)
		{
			return Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				(*texture_coords_)[0],
				(*texture_coords_)[1],
				(*texture_coords_)[2],
				material_,
				*normal_, 
				material_->transparency_,
				material_->refractive_index_);
		}
		else
		{
			return Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				material_,
				*normal_, 
				material_->transparency_,
				material_->refractive_index_);
		}
	}
	else
	{
		if (texture_coords_->size() == 3)
		{
			return Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				(*texture_coords_)[0],
				(*texture_coords_)[1],
				(*texture_coords_)[2],
				material_,
				material_->transparency_,
				material_->refractive_index_);
		}
		else
		{
			return Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				material_,
				material_->transparency_,
				material_->refractive_index_);
		}
	}
}
