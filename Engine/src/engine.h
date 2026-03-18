#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <fstream>
#include <algorithm>
#include <glaze/core/read.hpp>

#include "headers/renderer.h"
#include "headers/resource_manager.h"
#include "headers/scene.h"
#include "_json_parser.h"

struct ScenePair {
	std::string name{};
	std::shared_ptr<Scene> scene{};
};

namespace Engine
{
	namespace {
		bool engineInit = false;
		Renderer engine_Renderer{};
		ResourceManager engine_RM{};
		std::string_view json_folder_path{};

		std::vector<ScenePair> EngineScenes{};
		ScenePair currentScene{};
		std::string buffer{};

		glz::error_code ParseManifest(std::string_view scenePath, ManifestConfig& container)
		{
			buffer.clear();
			auto error = glz::read_file_json(container, scenePath, buffer);
			if (error) {
				std::cout << "ERROR_PARSING_JSON::AT_SCENE::" << scenePath << std::endl;
				return error.ec;
			}

			return {};
		};

		glz::error_code ParseEntities(std::string_view scenePath, SceneEntities& container)
		{
			buffer.clear();
			auto error = glz::read_file_json(container, scenePath, buffer);
			if (error) {
				std::cout << "ERROR_PARSING_JSON::AT_SCENE::" << scenePath << std::endl;
				return error.ec;
			}

			return {};
		};

		glz::error_code WriteEntities(ScenePair& scene)
		{
			SceneEntities container{};

			for (Entity e : scene.scene.get()->GetEntityList()) {
				EntityConfig temp{};
				if (auto it = scene.scene.get()->GetComponent<CameraComponent>(e)) {
					temp.camera = *it.get();
				}

				if (auto it = scene.scene.get()->GetComponent<HierarchyComponent>(e)) {
					temp.hierarchy = *it.get();
				}

				if (auto it = scene.scene.get()->GetComponent<LightComponent>(e)) {
					temp.light = *it.get();
				}

				if (auto it = scene.scene.get()->GetComponent<RenderComponent>(e)) {
					temp.render = *it.get();
				}

				if (auto it = scene.scene.get()->GetComponent<TransformComponent>(e)) {
					temp.transform = *it.get();
				}

				container.entities.emplace_back(temp);
			}

			buffer.clear();
			auto error = glz::write_file_json(container, std::string(json_folder_path) + std::string(scene.name) + ".json", buffer);
			if (!error) {
				std::cout << "ERROR_WRITING_TO_JSON::AT_SCENE::" << scene.name << std::endl;
				return error.ec;
			}

			return {};
		}

		std::string_view getFileName(std::string_view path)
		{
			size_t off = path.find_last_of("/");
			size_t end = path.find_last_of(".", path.length() - 1, path.length() - off - 1);

			if (end == -1) {
				return path.substr(off + 1, path.length() - 1);
			}
			else {
				return path.substr(off + 1 , end - off - 1);
			}
		}
	}

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
};

#endif