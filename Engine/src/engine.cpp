#include "engine.h"

#include <iostream>
#include <algorithm>
#include <format>

ScenePair Engine::InitializeEngine(unsigned int WIDTH, unsigned int HEIGHT, std::string_view jsonPath)
{
	if (engineInit) {
		std::cout << "BAD_FUNC_CALL::InitializeEngine::ENGINE_ALREADY_INITIALIZED" << std::endl;
		return ScenePair("", nullptr);
	}

	engine_Renderer.InitRenderer(WIDTH, HEIGHT);
	json_folder_path = jsonPath;

	engineInit = true;

	std::shared_ptr<Scene> temp = std::make_shared<Scene>();
	EngineScenes.emplace_back(ScenePair("StartScene", temp));
	return EngineScenes.back();
}

ScenePair Engine::InitializeEngine(unsigned int WIDTH, unsigned int HEIGHT, std::vector<std::string_view> SceneNames, std::string_view startScene, std::string_view jsonPath)
{
	if (engineInit) {
		std::cout << "BAD_FUNC_CALL::InitializeEngine::ENGINE_ALREADY_INITIALIZED" << std::endl;
		return ScenePair("", nullptr);
	}

	engine_Renderer.InitRenderer(WIDTH, HEIGHT);
	json_folder_path = jsonPath;

	ScenePair temp = ScenePair("StartScene", std::make_shared<Scene>());
	for (std::string_view s : SceneNames) {
		EngineScenes.emplace_back(Engine::LoadScene_json(s));

		if (std::string(s).compare(std::string(startScene)) == 0) {
			temp = EngineScenes.back();
			temp.scene->Load(engine_RM, engine_Renderer);
		}
	}

	engineInit = true;
	currentScene = temp;

	return temp;
}

ScenePair Engine::GetCurrentScene()
{
	return currentScene;
}

ScenePair Engine::SwitchScenes(std::string_view sceneName)
{
	if (currentScene.name.compare(std::string(sceneName)) == 0)
		return currentScene;

	for (int i = 0; i < EngineScenes.size(); i++) {
		if (EngineScenes[i].name == sceneName) {
			if (currentScene.scene)
				currentScene.scene.get()->Unload();

			ScenePair temp = EngineScenes[i];

			temp.scene.get()->Load(engine_RM, engine_Renderer);
			currentScene = temp;

			return currentScene;
		}
	}

	return ScenePair();
}

ScenePair Engine::LoadScene_json(std::string_view sceneName)
{
	Scene newScene{};

	// First, load manifest data.
	ManifestConfig manifest{};
	std::string path = std::string(json_folder_path) + std::string(sceneName);
	if (auto ec = ParseManifest(path + "_manifest.json", manifest) != glz::error_code::none) {
		std::cout << "PARSE_ERROR::MANIFEST::AT_SCENE::" << sceneName << "::" << ec << std::endl;
		return ScenePair("", nullptr);
	}

	std::vector<uint32_t> shaderIDs{};
	for (ShaderManifestConfig smc : manifest.shader_manifest) {
		std::string name = std::format("{}_{}_{}", getFileName(smc.fragment_path), getFileName(smc.vertex_path), 
			smc.geo_path.has_value() ? getFileName(smc.geo_path.value()) : "");

		std::cout << std::string(smc.vertex_path).c_str() << std::endl;
		shaderIDs.emplace_back(
			Engine::engine_RM.LoadShader(std::string(smc.vertex_path).c_str(), std::string(smc.fragment_path).c_str(),
				name, smc.geo_path.has_value() ? std::string(smc.geo_path.value()).c_str() : nullptr)
		);
	}

	std::vector<uint32_t> modelIDs{};
	for (std::string_view mm : manifest.model_manifest) {
		AssetRegistry::RegisterPath(mm);
		modelIDs.emplace_back(
			Engine::engine_RM.LoadModel(mm, Engine::engine_Renderer.GetDynamicVBO())
		);
	}

	std::vector<uint32_t> textureIDs{};
	for (TextureManifestConfig tm : manifest.texture_manifest) {
		AssetRegistry::RegisterPath(tm.tex_path);
		textureIDs.emplace_back(
			Engine::engine_RM.LoadTexture_ID(std::string(tm.tex_path), tm.type)
		);
	}

	newScene.m_manager.get()->SetShaderIDs(shaderIDs, Engine::engine_RM);
	newScene.m_manager.get()->SetModelIDs(modelIDs, Engine::engine_RM);

	// Next, load entity data
	SceneEntities entities{};
	if (auto ec = ParseEntities(path + ".json", entities) != glz::error_code::none) {
		std::cout << "PARSE_ERROR::ENTITY::AT_SCENE::" << sceneName << "::" << ec << std::endl;
		return ScenePair("", nullptr);
	}

	for (EntityConfig ec : entities.entities) {
		Entity e = newScene.AddEntity();

		if (ec.camera.has_value()) {
			newScene.AddComponent<CameraComponent>(e, std::move(ec.camera.value()));
		}

		if (ec.hierarchy.has_value()) {
			newScene.AddComponent<HierarchyComponent>(e, std::move(ec.hierarchy.value()));
		}

		if (ec.light.has_value()) {
			LightType t = ec.light.value().type;
			newScene.AddComponent<LightComponent>(e, std::move(ec.light.value()), t);
		}

		if (ec.render.has_value()) {
			newScene.AddComponent<RenderComponent>(e, std::move(ec.render.value()));
		}

		if (ec.transform.has_value()) {
			newScene.AddComponent<TransformComponent>(e, std::move(ec.transform.value()));
		}
	}

	std::shared_ptr<Scene> temp = std::make_shared<Scene>(std::move(newScene));
	return ScenePair(std::string(sceneName), temp);
}

void Engine::SaveScene(const std::string& name)
{
	for (int i = 0; i < EngineScenes.size(); i++) {
		if (EngineScenes[i].name == name) {
			WriteEntities(EngineScenes[i]);
			return;
		}
	}
}

void Engine::SaveCurrentScene()
{
	WriteEntities(currentScene);
}

void Engine::SaveAllScenes()
{
	for (ScenePair s : EngineScenes) {
		WriteEntities(s);
	}
}

std::shared_ptr<CameraComponent> Engine::SetCurrentCamera(Entity e)
{
	if (!currentScene.scene)
		return nullptr;

	std::shared_ptr<CameraComponent> temp = currentScene.scene.get()->SetCamera(e);
	if (!temp) {
		std::cout << "SET_CAMERA_ERROR::THIS_CAMERA_COMPONENT_DOES_NOT_EXIST" << std::endl;
		return nullptr;
	}

	currentScene.scene.get()->SetCamUBO(800, 600, Engine::engine_Renderer);
	return temp;
}

void Engine::Render()
{
	if (currentScene.scene) {
		currentScene.scene->RenderScene(Engine::engine_Renderer);
	}
}