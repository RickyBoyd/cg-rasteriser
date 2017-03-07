#include "Utilities.h"
#include <glm/detail/type_vec3.hpp>
#include <algorithm>
#include <glm/detail/func_common.hpp>

Pixel Pixel::operator + (const Pixel p) const {
	return Pixel{
		pos + p.pos,
		z_inv + p.z_inv,
		illumination + p.illumination
	};
}

Pixel Pixel::operator - (const Pixel p) const {
	return Pixel{
		pos - p.pos,
		z_inv - p.z_inv,
		illumination - p.illumination
	};
}

Pixel Pixel::operator / (const float divisor) const {
	return Pixel{
		glm::vec2(pos) / divisor,
		z_inv / divisor,
		illumination / divisor
	};
}

glm::ivec2 Pixel::ipos() const
{
	return glm::round(pos);
}

void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result)
{
	int N = result.size();
	auto step = (b - a) / float(std::max(N - 1, 1));

	auto current = a;
	for (int i = 0; i < N; ++i)
	{
		result[i] = current;
		current = current + step;
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