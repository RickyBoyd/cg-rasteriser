#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	glm::vec3 pos;
};

struct Pixel
{
	glm::ivec2 pos;
	float z_inv;
};

void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result);
void Interpolate(const glm::ivec2 a, const glm::ivec2 b, std::vector<glm::ivec2>& result);
void Interpolate(const float a, const float b, std::vector<float> &result);