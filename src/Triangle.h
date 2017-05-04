#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include "Material.h"

class Triangle
{
public:
	glm::vec4 v0_;
	glm::vec4 v1_;
	glm::vec4 v2_;
	glm::vec2 vt0_;
	glm::vec2 vt1_;
	glm::vec2 vt2_;
	glm::vec3 normal_;
	std::shared_ptr<Material> material_;
	float transparency_;
	float refractive_index_;

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, float transparency, float refractive_index);

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, glm::vec3 normal, float transparency, float refractive_index);

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material, glm::vec3 normal, float transparency, float refractive_index);

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material,float transparency, float refractive_index);

	// Member functions definitions including constructor
	Triangle(void);

	void ComputeNormal();

	glm::vec3 GetAmbientColour(glm::vec2 uv) const ;
	
	glm::vec3 GetDiffuseColour(glm::vec2 uv) const ;
	
	glm::vec3 GetSpecularColour(glm::vec2 uv) const ;
	
	float GetSpecularIntensity(glm::vec2 uv) const ;

	float GetSpecularExponent(glm::vec2 uv) const ;
	
	
};