#include "Utilities.h"
#include <glm/detail/type_vec3.hpp>
#include <algorithm>

void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result)
{
	int N = result.size();
	glm::vec3 a_vec(a.pos.x, a.pos.y, a.z_inv);
	glm::vec3 b_vec(b.pos.x, b.pos.y, b.z_inv);
	glm::vec3 step = glm::vec3(b_vec - a_vec) / float(std::max(N - 1, 1));
	glm::vec3 current(a_vec);
	for (int i = 0; i < N; ++i)
	{
		result[i] = { glm::ivec2(round(current.x), round(current.y)), current.z };
		current += step;
	}
}

void Interpolate(const glm::ivec2 a, const glm::ivec2 b, std::vector<glm::ivec2>& result)
{
	int N = result.size();
	glm::vec2 step = glm::vec2(b - a) / float(std::max(N - 1, 1));
	glm::vec2 current(a);
	for (int i = 0; i < N; ++i)
	{
		result[i] = current;
		current += step;
	}
}

void Interpolate(const float a, const float b, std::vector<float> &result)
{
	switch (result.size())
	{
	case 0:
		break;
	case 1:
		result[0] = (a + b) / 2.0f;
		break;
	default:
		float step = (b - a) / float(result.size() - 1);
		float current = a;
		for (int i = 0; i < result.size(); i++) {
			result[i] = current;
			current += step;
		}
		break;
	}
}