#pragma once
#include <glm/detail/type_vec3.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <regex>
#include "Triangle.h"
#include "Material.h"
#include "Face.h"


class Model
{
public:
	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec3> normals_;
	std::vector<glm::vec2> texture_coords_;
	std::vector<std::shared_ptr<Material>> materials_;
	std::vector<std::shared_ptr<Face>> faces_;

	Model(std::vector<std::shared_ptr<Face>> faces);

	Model(std::string filename);

	///<summary>Return a vector of the substrings when str is split at delimiters matching regex</summary>
	static std::vector<std::string> SplitString(const std::string& str, const std::string& regex);

	std::vector<Triangle> ToTriangles(const glm::vec3 transform = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)) const;
};
