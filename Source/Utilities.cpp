#include "Utilities.h"
#include <glm/detail/type_vec3.hpp>
#include <algorithm>
#include <glm/detail/func_common.hpp>


void Interpolate(const Pixel a, const Pixel b, std::vector<Pixel>& result)
{
	int N = result.size();
	float divisor_step = float(std::max(N - 1, 1));
	auto step_pos = glm::vec2(b.pos - a.pos) / divisor_step;
	auto step_z_inv = (b.z_inv - a.z_inv) / divisor_step;
	auto step_camera_pos = (b.camera_pos - a.camera_pos) / divisor_step;
	auto step_normal = (b.camera_normal - a.camera_normal) / divisor_step; // Not strictly necessary if we're interpolating triangles
	auto step_diffuse_reflectance = (b.diffuse_reflectance - a.diffuse_reflectance) / divisor_step;
	auto step_indirect_reflectance = (b.indirect_reflectance - a.indirect_reflectance) / divisor_step;

	auto current_pos = glm::vec2(a.pos);
	auto current_z_inv = a.z_inv;
	auto current_camera_pos = a.camera_pos;
	auto current_normal = a.camera_normal;
	auto current_diffuse_reflectance = a.diffuse_reflectance;
	auto current_indirect_reflectance = a.indirect_reflectance;
	for (int i = 0; i < N; ++i)
	{
		result[i] = Pixel { glm::round(current_pos), current_z_inv, current_camera_pos, current_normal, current_diffuse_reflectance, current_indirect_reflectance };
		current_pos += step_pos;
		current_z_inv += step_z_inv;
		current_camera_pos += step_camera_pos;
		current_normal += step_normal;
		current_diffuse_reflectance += step_diffuse_reflectance;
		current_indirect_reflectance += step_indirect_reflectance;
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