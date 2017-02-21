#pragma once
#include "Model.h"
#include "Light.h"
#include "Camera.h"

struct ModelInstance
{
	ModelInstance(const Model& model, const glm::vec3 transform = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f))
		: model(model), transform(transform), scale_(scale)
	{
	}

	Model model;
	glm::vec3 transform;
	glm::vec3 scale_;
};

class Scene
{
public:
	std::vector<ModelInstance> model_instances_;
	std::vector<Light> lights_;
	Camera camera_;

	Scene(std::vector<ModelInstance> model_instances, std::vector<Light> lights_, Camera camera);
	~Scene();

	std::vector<Triangle> ToTriangles() const;
};

