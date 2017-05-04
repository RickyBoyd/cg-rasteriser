#pragma once
#include <glm/detail/type_vec3.hpp>
#include <vector>
#include <memory>
#include <string>

enum IlluminationModel
{
	ColourOnAmbientOff = 0,
	ColourOnAmbientOn = 1,
	HighlightOn = 2,
	ReflectionOnRaytraceOn = 3,
	TransparencyGlassOnReflectionRaytraceOn = 4,
	ReflectionFresnelOnRaytraceOn = 5,
	TransparencyRefractionOnReflectionFresnelOffRaytraceOn = 6,
	TransparencyRefractionOnReflectionFresenelOnRaytraceOn = 7,
	ReflectionOnRaytraceOff = 8,
	TransparencyGlassOnReflectionRaytraceOff = 9,
	CastsShadowsOntoInvisibleSurfaces = 10
};

class Material
{
public:
	std::string name_;
	glm::vec3 ambient_colour_;
	glm::vec3 diffuse_colour_;
	glm::vec3 specular_colour_;
	float specular_exponent_ = 50.0f;
	float transparency_ = 0.0f;
	IlluminationModel illumination_model_;
	float refractive_index_ = 1.0f;

	unsigned char* ambient_texture_ = nullptr;
	int ambient_texture_x_, ambient_texture_y_, ambient_texture_n_;
	unsigned char* diffuse_texture_ = nullptr;
	int diffuse_texture_x_, diffuse_texture_y_, diffuse_texture_n_;
	unsigned char* specular_colour_texture_ = nullptr;
	int specular_colour_texture_x_, specular_colour_texture_y_, specular_colour_texture_n_;
	unsigned char* specular_intensity_texture_ = nullptr;
	int specular_intensity_texture_x_, specular_intensity_texture_y_, specular_intensity_texture_n_;


	Material();
	Material(std::string name, glm::vec3 ambient_colour, glm::vec3 diffuse_colour, glm::vec3 specular_colour, float specular_exponent, float transparency);
	~Material();

	glm::vec3 GetAmbientColour(float u, float v) const;
	glm::vec3 GetDiffuseColour(float u, float v) const;
	glm::vec3 GetSpecularColour(float u, float v) const;
	float GetSpecularIntensity(float u, float v) const;

	static std::vector<std::shared_ptr<Material> > LoadMaterials(std::string filename);
	static std::vector<std::string> SplitString(const std::string& str, const std::string& regex);
};

