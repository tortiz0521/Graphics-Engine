#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <fstream>
#include <algorithm>
#include <glaze/core/read.hpp>

#include "../src/renderer/renderer.h"
#include "../src/resource/resource_manager.h"
#include "../src/scene/scene.h"
#include "../src/util/_json_parser.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/emscripten.h>
#endif

struct ScenePair {
	std::string name{};
	std::shared_ptr<Scene> scene{};
};

namespace Engine
{
	ScenePair InitializeEngine(unsigned int WIDTH, unsigned int HEIGHT, std::string_view jsonPath);
	ScenePair InitializeEngine(unsigned int WIDTH, unsigned int HEIGHT, std::vector<std::string_view> SceneNames, std::string_view startScene, std::string_view jsonPath);

	ScenePair SwitchScenes(std::string_view sceneName);
	ScenePair GetCurrentScene();
	ScenePair LoadScene_json(std::string_view scene);
	ScenePair CreateScene(std::string_view name);
	void SaveScene(const std::string& name);
	void SaveCurrentScene();
	void SaveAllScenes();

	std::shared_ptr<CameraComponent> SetCurrentCamera(Entity e);
	void Render();
	bool WindowClosed();
	void ShutDownEngine();
};

#endif