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
	
	/*Vertex operator + (const Vertex v) const {
		return Vertex{
			pos + v.pos,
			normal + v.normal,
			diffuse_reflectance + v.diffuse_reflectance,
			indirect_reflectance + v.indirect_reflectance
		};
	}

	Vertex operator - (const Vertex v) const {
		return Vertex{
			pos - v.pos,
			normal - v.normal,
			diffuse_reflectance - v.diffuse_reflectance,
			indirect_reflectance - v.indirect_reflectance
		};
	}

	Vertex operator / (const float divisor) const {
		return Vertex{
			pos / divisor,
			normal / divisor,
			diffuse_reflectance / divisor,
			indirect_reflectance / divisor
		};
	}*/
};

struct Pixel
{
	glm::vec2 pos;
	float z_inv;
	glm::vec3 illumination;

	glm::ivec2 ipos() const;

	Pixel operator + (const Pixel p) const;

	Pixel operator - (const Pixel p) const;

	Pixel operator / (const float divisor) const;
};

void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result);
void Interpolate(const glm::ivec2 a, const glm::ivec2 b, std::vector<glm::ivec2>& result);
void Interpolate(const float a, const float b, std::vector<float> &result);