#include "Utilities.h"
#include <glm/detail/type_vec3.hpp>
#include <algorithm>
#include <glm/detail/func_common.hpp>


void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result)
{
	int N = result.size();
	auto step_pos = glm::vec2(b.pos - a.pos) / float(std::max(N - 1, 1));
	auto step_z_inv = (b.z_inv - a.z_inv) / float(std::max(N - 1, 1));
	auto step_illumination = (b.illumination - a.illumination) / float(std::max(N - 1, 1));

	auto current_pos = glm::vec2(a.pos);
	auto current_z_inv = a.z_inv;
	auto current_illumination = a.illumination;
	for (int i = 0; i < N; ++i)
	{
		result[i] = Pixel { glm::round(current_pos), current_z_inv, current_illumination };
		current_pos += step_pos;
		current_z_inv += step_z_inv;
		current_illumination += step_illumination;
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