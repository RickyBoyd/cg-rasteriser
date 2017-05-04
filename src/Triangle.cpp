#include "Triangle.h"

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, float transparency = 0.0f, float refractive_index = 1.0f)
	: transparency_(transparency), refractive_index_(refractive_index), material_(material)
{
	v0_ = glm::vec4(v0, 1.0f);
	v1_ = glm::vec4(v1, 1.0f);
	v2_ = glm::vec4(v2, 1.0f);
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, glm::vec3 normal, float transparency = 0.0f, float refractive_index = 1.0f)
	: transparency_(transparency), refractive_index_(refractive_index), material_(material), normal_(normal)
{
	v0_ = glm::vec4(v0, 1.0f);
	v1_ = glm::vec4(v1, 1.0f);
	v2_ = glm::vec4(v2, 1.0f);
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material, glm::vec3 normal, float transparency = 0.0f, float refractive_index = 1.0f)
	: normal_(normal), transparency_(transparency), refractive_index_(refractive_index), vt0_(vt0), vt1_(vt1), vt2_(vt2), material_(material)
{
	v0_ = glm::vec4(v0, 1.0f);
	v1_ = glm::vec4(v1, 1.0f);
	v2_ = glm::vec4(v2, 1.0f);
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material,float transparency = 0.0f, float refractive_index = 1.0f)
	:  transparency_(transparency), refractive_index_(refractive_index), vt0_(vt0), vt1_(vt1), vt2_(vt2), material_(material)
{
	v0_ = glm::vec4(v0, 1.0f);
	v1_ = glm::vec4(v1, 1.0f);
	v2_ = glm::vec4(v2, 1.0f);
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
	normal_ = glm::normalize(glm::cross(e2, e1));
}

glm::vec3 Triangle::GetAmbientColour(glm::vec2 uv) const 
{
	return material_->GetAmbientColour(uv.x, uv.y);
}

glm::vec3 Triangle::GetDiffuseColour(glm::vec2 uv) const 
{
	return material_->GetDiffuseColour(uv.x, uv.y);
}

glm::vec3 Triangle::GetSpecularColour(glm::vec2 uv) const 
{
	return material_->GetSpecularColour(uv.x, uv.y);
}

float Triangle::GetSpecularIntensity(glm::vec2 uv) const 
{
	return material_->GetSpecularIntensity(uv.x, uv.y);
}

float Triangle::GetSpecularExponent(glm::vec2 uv) const 
{
	return material_->specular_exponent_;
}