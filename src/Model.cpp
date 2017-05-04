#include "Model.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>

Model::Model(std::vector<std::shared_ptr<Face>> faces) : faces_(faces) {}

Model::Model(std::string filename)
{
	// TODO: implement support for groups/objects
	auto path = boost::filesystem::path(filename);
	std::ifstream is(path.string());

	std::shared_ptr<Material> default_material = std::make_shared<Material>("default", glm::vec3(0.75f, 0, 0.75f), glm::vec3(0.75f, 0, 0.75f), glm::vec3(0.75f, 0, 0.75f), 50.0f, 0.0f);
	std::shared_ptr<Material> material = default_material;

	for (std::string str; std::getline(is, str);)
	{
		std::vector<std::string> tokens;
		boost::trim_all(str);
		boost::split(tokens, str, boost::is_any_of("\t "));

		if (tokens[0].compare("v") == 0)
		{
			auto vertex = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			vertices_.push_back(vertex);
		}
		else if (tokens[0].compare("f") == 0)
		{
			std::vector<std::string> vertex1, vertex2, vertex3;
			boost::split(vertex1, tokens[1], boost::is_any_of("/"));
			boost::split(vertex2, tokens[2], boost::is_any_of("/"));
			boost::split(vertex3, tokens[3], boost::is_any_of("/"));

			std::vector<glm::vec3> vertices{};
			auto texture_coords = std::vector<glm::vec2>{};
			glm::vec3 normal;

			// If vertex coordinates have first part, retrieve the vertices for the face
			if (vertex1.size() >= 1 && vertex2.size() >= 1 && vertex3.size() >= 1)
			{
				try
				{
					vertices.push_back(vertices_[std::stoi(vertex1[0]) - 1]);
					vertices.push_back(vertices_[std::stoi(vertex2[0]) - 1]);
					vertices.push_back(vertices_[std::stoi(vertex3[0]) - 1]);
				}
				catch (std::exception e) {}
			}

			// If vertex coordinates have second part, retrieve the texture coordinates for the face
			if (vertex1.size() >= 2 && vertex2.size() >= 2 && vertex3.size() >= 2)
			{
				try
				{
					auto coords = std::vector<glm::vec2>{};
					coords.push_back(texture_coords_[std::stoi(vertex1[1]) - 1]);
					coords.push_back(texture_coords_[std::stoi(vertex2[1]) - 1]);
					coords.push_back(texture_coords_[std::stoi(vertex3[1]) - 1]);

					texture_coords = coords;
				}
				catch (std::exception e) {}
			}

			// If vertex coordinates have third part, retrieve the normal for the face
			if (vertex1.size() >= 3 && vertex2.size() >= 3 && vertex3.size() >= 3)
			{
				try
				{
					normal = normals_[std::stoi(vertex1[2]) - 1];
				}
				catch (std::exception e) {}
			} else
			{
				glm::vec3 e1 = vertices[1] - vertices[0];
				glm::vec3 e2 = vertices[2] - vertices[0];
				normal = glm::normalize(glm::cross(e2, e1));
			}

			auto face = Face(vertices, std::make_shared<std::vector<glm::vec2>>(texture_coords), std::make_shared<glm::vec3>(normal), material);
			faces_.push_back(std::make_shared<Face>(face));
		}
		else if (tokens[0].compare("vn") == 0)
		{
			auto normal = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			normals_.push_back(normal);
		}
		else if (tokens[0].compare("vt") == 0)
		{
			auto texture_coord = glm::vec2(std::stof(tokens[1]), std::stof(tokens[2]));
			texture_coords_.push_back(texture_coord);
		}
		else if (tokens[0].compare("mtllib") == 0)
		{
			boost::filesystem::path mtl_path = path.parent_path() / boost::filesystem::path(tokens[1]);
			auto materials = Material::LoadMaterials(mtl_path.string());
			materials_.insert(materials_.end(), materials.begin(), materials.end());
		}
		else if (tokens[0].compare("usemtl") == 0)
		{
			std::vector<std::shared_ptr<Material>>::iterator matching = std::find_if(materials_.begin(), materials_.end(), [tokens](std::shared_ptr<Material> m) -> bool { return m->name_.compare(tokens[1]) == 0; });
			material = *matching;
		}
	}
}

std::vector<Triangle> Model::ToTriangles(const glm::vec3 transform, const glm::vec3 scale) const
{
	std::vector<Triangle> tris;
	tris.reserve(faces_.size());
	for (auto face : faces_)
	{
		tris.push_back(face->ToTriangle(transform, scale));
	}
	return tris;
}
