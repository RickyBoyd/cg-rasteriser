#include "Material.h"
#include <regex>
#include <fstream>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Material::Material() {}

Material::Material(std::string name, glm::vec3 ambient_colour, glm::vec3 diffuse_colour, glm::vec3 specular_colour, float specular_exponent, float transparency)
	: name_(name), ambient_colour_(ambient_colour), diffuse_colour_(diffuse_colour), specular_colour_(specular_colour), specular_exponent_(specular_exponent), transparency_(transparency)
{
}


Material::~Material()
{
}

glm::vec3 Material::GetAmbientColour(float u, float v) const
{
	if (ambient_texture_ == nullptr || u >= 1.0 || v >= 1.0 || u < 0.0 || v < 0.0)
	{
		return ambient_colour_;
	}
	int x = round(u * ambient_texture_x_);
	int y = round(v * ambient_texture_y_);
	return glm::vec3(
		float(ambient_texture_[(y * ambient_texture_x_ * ambient_texture_n_) + (x * ambient_texture_n_)]) / 255.0f,
		float(ambient_texture_[(y * ambient_texture_x_ * ambient_texture_n_) + (x * ambient_texture_n_) + 1]) / 255.0f,
		float(ambient_texture_[(y * ambient_texture_x_ * ambient_texture_n_) + (x * ambient_texture_n_) + 2]) / 255.0f);
}

glm::vec3 Material::GetDiffuseColour(float u, float v) const
{
	if (diffuse_texture_ == nullptr || u >= 1.0 || v >= 1.0 || u < 0.0 || v < 0.0)
	{
		return diffuse_colour_;
	}
	int x = round(u * ambient_texture_x_);
	int y = round(v * ambient_texture_y_);
	return glm::vec3(
		float(diffuse_texture_[(y * diffuse_texture_x_ * diffuse_texture_n_) + (x * diffuse_texture_n_)]) / 255.0f,
		float(diffuse_texture_[(y * diffuse_texture_x_ * diffuse_texture_n_) + (x * diffuse_texture_n_) + 1]) / 255.0f,
		float(diffuse_texture_[(y * diffuse_texture_x_ * diffuse_texture_n_) + (x * diffuse_texture_n_) + 2]) / 255.0f);
}

glm::vec3 Material::GetSpecularColour(float u, float v) const
{
	if (specular_colour_texture_ == nullptr || u >= 1.0 || v >= 1.0 || u < 0.0 || v < 0.0)
	{
		return specular_colour_;
	}
	int x = round(u * specular_colour_texture_x_);
	int y = round(v * specular_colour_texture_y_);
	return glm::vec3(
		float(specular_colour_texture_[(y * specular_colour_texture_x_ * specular_colour_texture_n_) + (x * specular_colour_texture_n_)]) / 255.0f,
		float(specular_colour_texture_[(y * specular_colour_texture_x_ * specular_colour_texture_n_) + (x * specular_colour_texture_n_) + 1]) / 255.0f,
		float(specular_colour_texture_[(y * specular_colour_texture_x_ * specular_colour_texture_n_) + (x * specular_colour_texture_n_) + 2]) / 255.0f);
}

float Material::GetSpecularIntensity(float u, float v) const
{
	if (specular_intensity_texture_ == nullptr || u >= 1.0 || v >= 1.0 || u < 0.0 || v < 0.0)
	{
		return 1.0f; // TODO: is this actually available in the MTL format or do you just rely on Ks?
	}
	int x = round(u * specular_intensity_texture_x_);
	int y = round(v * specular_intensity_texture_y_);
	return float(specular_intensity_texture_[(y * specular_intensity_texture_x_ * specular_intensity_texture_n_) + (x * specular_intensity_texture_n_)]) / 255.0f;

}

std::vector<std::shared_ptr<Material>> Material::LoadMaterials(std::string filename)
{
	std::ifstream is(filename);
	auto path = boost::filesystem::path(filename);

	std::vector<std::shared_ptr<Material>> materials;
	Material *material = nullptr;

	for (std::string str; std::getline(is, str);)
	{
		std::vector<std::string> tokens;
		boost::trim_all(str);
		boost::split(tokens, str, boost::is_any_of("\t "));
		std::remove_if(tokens.begin(), tokens.end(), [](std::string t) -> bool { return t.compare("") == 0; });

		if (tokens[0].compare("newmtl") == 0)
		{
			if (material != nullptr)
			{
				materials.push_back(std::shared_ptr<Material>(material));
			}
			material = new Material();
			material->name_ = tokens[1];
		}
		else if (tokens[0].compare("Ka") == 0)
		{
			auto rgb = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			material->ambient_colour_ = rgb;
		}
		else if (tokens[0].compare("Kd") == 0)
		{
			auto rgb = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			material->diffuse_colour_ = rgb;
		}
		else if (tokens[0].compare("Ks") == 0)
		{
			auto rgb = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			material->specular_colour_ = rgb;
		}
		else if (tokens[0].compare("Ns") == 0)
		{
			material->specular_exponent_ = std::stof(tokens[1]);
		}
		else if (tokens[0].compare("d") == 0)
		{
			material->transparency_ = 1 - std::stof(tokens[1]);
		}
		else if (tokens[0].compare("Tr") == 0)
		{
			material->transparency_ = std::stof(tokens[1]);
		}
		else if (tokens[0].compare("illum") == 0)
		{
			material->illumination_model_ = static_cast<IlluminationModel>(std::stoi(tokens[1]));
		}
		else if (tokens[0].compare("Ni") == 0)
		{
			material->refractive_index_ = std::stof(tokens[1]);
		}
		else if (tokens[0].compare("map_Ka") == 0)
		{
			boost::filesystem::path texture_path = path.parent_path() / boost::filesystem::path(tokens[1]);
			material->ambient_texture_ = stbi_load(texture_path.string().c_str(), &material->ambient_texture_x_, &material->ambient_texture_y_, &material->ambient_texture_n_, 0);
		}
		else if (tokens[0].compare("map_Kd") == 0)
		{
			boost::filesystem::path texture_path = path.parent_path() / boost::filesystem::path(tokens[1]);
			material->diffuse_texture_ = stbi_load(texture_path.string().c_str(), &material->diffuse_texture_x_, &material->diffuse_texture_y_, &material->diffuse_texture_n_, 0);
		}
		else if (tokens[0].compare("map_Ks") == 0)
		{
			boost::filesystem::path texture_path = path.parent_path() / boost::filesystem::path(tokens[1]);
			material->specular_colour_texture_ = stbi_load(texture_path.string().c_str(), &material->specular_colour_texture_x_, &material->specular_colour_texture_y_, &material->specular_colour_texture_n_, 0);
		}
		else if (tokens[0].compare("map_Ns") == 0)
		{
			boost::filesystem::path texture_path = path.parent_path() / boost::filesystem::path(tokens[1]);
			material->specular_intensity_texture_ = stbi_load(texture_path.string().c_str(), &material->specular_intensity_texture_x_, &material->specular_intensity_texture_y_, &material->specular_intensity_texture_n_, 0);
		}
		else if (tokens[0].compare("map_d") == 0)
		{

		}
		else if (tokens[0].compare("map_bump") == 0 || tokens[0].compare("bump"))
		{

		}
		else if (tokens[0].compare("disp") == 0)
		{

		}
		else if (tokens[0].compare("decal") == 0)
		{

		}
	}

	if (material != nullptr)
	{
		materials.push_back(std::shared_ptr<Material>(material));
	}

	return materials;
}
