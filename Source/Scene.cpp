#include "Scene.h"



Scene::Scene(std::vector<ModelInstance> model_instances, std::vector<Light> lights, Camera camera) 
	: model_instances_(model_instances), lights_(lights), camera_(camera)
{
}


Scene::~Scene()
{
}

std::vector<Triangle> Scene::ToTriangles() const
{
	std::vector<Triangle> scene_tris;
	for (auto model_instance : model_instances_)
	{
		auto model_tris = model_instance.model.ToTriangles(model_instance.transform, model_instance.scale_);
		scene_tris.insert(scene_tris.end(), model_tris.begin(), model_tris.end());
	}

	return scene_tris;
}
