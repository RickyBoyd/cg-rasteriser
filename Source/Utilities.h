#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 diffuse_reflectance;
	glm::vec3 indirect_reflectance;
};

struct Pixel
{
	glm::ivec2 pos;
	float z_inv;
	glm::vec3 illumination;
};

void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result);
void Interpolate(const glm::ivec2 a, const glm::ivec2 b, std::vector<glm::ivec2>& result);
void Interpolate(const float a, const float b, std::vector<float> &result);