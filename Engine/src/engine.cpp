#include "../include/engine.h"

#include <iostream>
#include <algorithm>
#include <format>

namespace {
	bool engineInit = false;
	std::unique_ptr<Renderer> engine_Renderer{};
	std::unique_ptr<ResourceManager> engine_RM{};
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

		for (Entity e : scene.scene->GetEntityList()) {
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
	};

	std::string_view getFileName(std::string_view path)
	{
		size_t off = path.find_last_of("/");
		size_t end = path.find_last_of(".", path.length() - 1, path.length() - off - 1);

		if (end == -1) {
			return path.substr(off + 1, path.length() - 1);
		}
		else {
			return path.substr(off + 1, end - off - 1);
		}
	}
}

bool Engine::EngineReady()
{
	return engineInit;
}

ScenePair Engine::InitializeEngine(unsigned int WIDTH, unsigned int HEIGHT, std::string_view jsonPath)
{
	if (engineInit) {
		std::cout << "BAD_FUNC_CALL::InitializeEngine::ENGINE_ALREADY_INITIALIZED" << std::endl;
		return ScenePair("", nullptr);
	}

	engine_Renderer = std::make_unique<Renderer>();
	engine_RM = std::make_unique<ResourceManager>();

	json_folder_path = jsonPath;
	engineInit = true;
	std::shared_ptr<Scene> temp = std::make_shared<Scene>();
	EngineScenes.emplace_back(ScenePair("StartScene", temp));

	engine_Renderer->InitRenderer(WIDTH, HEIGHT);

	return EngineScenes.back();
}

ScenePair Engine::InitializeEngine(unsigned int WIDTH, unsigned int HEIGHT, std::vector<std::string_view> SceneNames, std::string_view startScene, std::string_view jsonPath)
{
	if (engineInit) {
		std::cout << "BAD_FUNC_CALL::InitializeEngine::ENGINE_ALREADY_INITIALIZED" << std::endl;
		return ScenePair("", nullptr);
	}

	engine_Renderer = std::make_unique<Renderer>();
	engine_RM = std::make_unique<ResourceManager>();
	engine_Renderer->InitRenderer(WIDTH, HEIGHT);

	json_folder_path = jsonPath;

	// Prepare a default start scene pair.
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	ScenePair start = ScenePair("StartScene", scene);

	// Vector used to track program IDs for renderer init.
	std::vector<uint32_t> IDs{};

	// Load all scenes and identify the requested start scene.
	for (std::string_view s : SceneNames) {
		EngineScenes.emplace_back(Engine::LoadScene_json(s));
		IDs.insert(IDs.end(), EngineScenes.back().scene->m_manager->m_shaderIDs.begin(), EngineScenes.back().scene->m_manager->m_shaderIDs.end());

		std::string name = std::string(s);
		if (name == std::string(startScene)) {
			start.name = name;
			start.scene = EngineScenes.back().scene;
			if (start.scene) {
				start.scene->Load(*engine_RM.get(), *engine_Renderer.get());
			}
		}
	}

#ifdef __EMSCRIPTEN__
	engine_Renderer->BindUBOs(IDs);
#endif

	engineInit = true;
	currentScene = start;

	return currentScene;
}

void Engine::ShutDownEngine()
{
	engine_RM->ReleaseResources();
	engine_Renderer->ShutDownRenderer();

	engine_RM.release();
	engine_Renderer.release();
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

			temp.scene.get()->Load(*engine_RM.get(), *engine_Renderer.get());
			currentScene = temp;

			return currentScene;
		}
	}

	return ScenePair();
}

ScenePair Engine::LoadScene_json(std::string_view sceneName)
{
	std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

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

#ifdef __EMSCRIPTEN__
		std::string v_path = std::string(getFileName(smc.vertex_path)) + "_es300.vs";
		std::string f_path = std::string(getFileName(smc.fragment_path)) + "_es300.fs";

		std::string g_path{};
		if (smc.geo_path.has_value())
			g_path = std::string(getFileName(smc.geo_path.value())) + "_es300.gs";

		shaderIDs.emplace_back(
			engine_RM->LoadShader(v_path.c_str(), f_path.c_str(),
				name, g_path.empty() ? nullptr : g_path.c_str())
		);
#else
		shaderIDs.emplace_back(
			engine_RM->LoadShader(std::string(smc.vertex_path).c_str(), std::string(smc.fragment_path).c_str(),
				name, smc.geo_path.has_value() ? std::string(smc.geo_path.value()).c_str() : nullptr)
		);
#endif
	}

	std::vector<uint32_t> modelIDs{};
	for (std::string_view mm : manifest.model_manifest) {
		AssetRegistry::RegisterPath(mm);
		modelIDs.emplace_back(
			engine_RM->LoadModel(mm, engine_Renderer->GetDynamicVBO())
		);
	}

	std::vector<uint32_t> textureIDs{};
	for (TextureManifestConfig tm : manifest.texture_manifest) {
		AssetRegistry::RegisterPath(tm.tex_path);
		textureIDs.emplace_back(
			engine_RM->LoadTexture_ID(std::string(tm.tex_path), tm.type)
		);
	}

	newScene->m_manager.get()->SetShaderIDs(shaderIDs, *engine_RM.get());
	newScene->m_manager.get()->SetModelIDs(modelIDs, *engine_RM.get());

	// Next, load entity data
	SceneEntities entities{};
	if (auto ec = ParseEntities(path + ".json", entities) != glz::error_code::none) {
		std::cout << "PARSE_ERROR::ENTITY::AT_SCENE::" << sceneName << "::" << ec << std::endl;
		return ScenePair(std::string(sceneName), newScene);
	}

	for (EntityConfig ec : entities.entities) {
		Entity e = newScene->AddEntity();

		if (ec.camera.has_value()) {
			newScene->AddComponent<CameraComponent>(e, std::move(ec.camera.value()));
		}

		if (ec.hierarchy.has_value()) {
			newScene->AddComponent<HierarchyComponent>(e, std::move(ec.hierarchy.value()));
		}

		if (ec.light.has_value()) {
			LightType t = ec.light.value().type;
			newScene->AddComponent<LightComponent>(e, std::move(ec.light.value()), t);
		}

		if (ec.render.has_value()) {
			newScene->AddComponent<RenderComponent>(e, std::move(ec.render.value()));
		}

		if (ec.transform.has_value()) {
			newScene->AddComponent<TransformComponent>(e, std::move(ec.transform.value()));
		}
	}

	return ScenePair(std::string(sceneName), newScene);
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

	currentScene.scene.get()->SetCamUBO(800, 600, *engine_Renderer.get());
	return temp;
}

void Engine::Render()
{
	if (currentScene.scene) {
		currentScene.scene->RenderScene(*engine_Renderer.get());
		//_CrtCheckMemory();
	}
}

bool Engine::WindowClosed()
{
#ifdef __EMSCRIPTEN__
	if (engine_Renderer->CloseWindow())
		emscripten_cancel_main_loop();
#else
	return engine_Renderer->CloseWindow();
#endif
}