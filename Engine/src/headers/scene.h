#ifndef SCENE_H
#define SCENE_H
#pragma once

#include "scene_object.h"
#include "renderer.h"

class Scene 
{
public:
	Scene() = default;
	void RenderScene(const Renderer& r);

	Scene& AddEntity(Entity && e);
	Scene& AddLight(Light && l);
	Scene& AddCamera(Camera && c);

private:
	std::vector<uint32_t> m_modelIDs, m_shaderIDs;

	// Entity lists
	std::vector<Entity> m_entities;
	std::vector<Light> m_lights;
	std::vector<Camera> m_cams;
	Camera m_currCam;
};


#endif