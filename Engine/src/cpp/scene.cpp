#include "../headers/scene.h"
#include <algorithm>


// We need to go through a few steps to render our scene properly:
//	- Get each light and send the appropriate data to the renderer based on the light type.
//	- Calculate the appropriate view based on the current scene camera.
//	- Load entites based on model or shader relationship (instancing)
void Scene::RenderScene(const Renderer& r)
{
	// First: get the lights to the renderer.
	for (int i = 0; i < m_lights.size(); i++) {

	}

	// Next: filter through entities in scene by model or shader used.
}