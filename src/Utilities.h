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
	glm::ivec2 pos; // 2d position on screen
	float z_inv;
	glm::vec3 camera_pos; // 3d position in camera coordinate system
	glm::vec3 camera_normal; // normal in camera coordinate system
	glm::vec2 vt;
};

void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result);
void Interpolate(const glm::ivec2 a, const glm::ivec2 b, std::vector<glm::ivec2>& result);
void Interpolate(const float a, const float b, std::vector<float> &result);