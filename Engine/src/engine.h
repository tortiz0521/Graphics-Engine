#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include "headers/renderer.h"
#include "headers/resource_manager.h"
#include "headers/scene.h"


namespace Engine
{
	namespace {
		bool engineInit = false;
		Renderer engineRenderer{};

		std::vector<Scene> EngineScenes{};
		std::shared_ptr<Scene> currentScene{};
	}

	void InitializeEngine();
	void InitializeEngine(std::vector<std::string_view> ScenePaths);

	Scene& SwitchScenes(Scene nextScene);
};

#endif