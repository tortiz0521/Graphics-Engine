#include "../headers/scene.h"
#include <algorithm>
#include <utility>

Scene::Scene()
{
	m_Cameras = std::make_shared<ComponentManager<CameraComponent>>();
	m_Hierarchies = std::make_shared<ComponentManager<HierarchyComponent>>();
	m_Lights = std::make_shared<ComponentManager<LightComponent>>();
	m_Renders = std::make_shared<ComponentManager<RenderComponent>>();
	m_Transforms = std::make_shared<ComponentManager<TransformComponent>>();

	m_manager = std::make_unique<SceneResourceManager>();
}

void Scene::Unload()
{
	m_manager.get()->SceneUnload();
}

void Scene::Load(ResourceManager& rm, Renderer& r)
{
	m_manager.get()->SceneLoad(rm);

	// Load scene components into component managers.
	cameras.reset(m_Cameras.get());
	hierarchy.reset(m_Hierarchies.get());
	lights.reset(m_Lights.get());
	renders.reset(m_Renders.get());
	transforms.reset(m_Transforms.get());

	if (m_uLight) {
		LightSystem::SortLights();
		m_uLight = false;
	}

	auto callback = [&](const LightVariant& l) -> void {
		r.AddUBOStruct(l);
	};
	LightSystem::PopulateLightUBOs(callback);

	if (m_uHier) {
		HierarchySystem::ResetHierarchy();
		m_uHier = false;
	}

	BatchingSystem::ResetBatch();
}

Entity Scene::AddEntity()
{
	Entity e = CreateEntity();
	std::cout << m_entities.size();
	m_entities.emplace_back(e);
	return e;
}

void Scene::RenderScene(Renderer& r)
{
	r.ProcessInput();
	HierarchySystem::UpdateHierarchySystem();

	// Update the UBOs in our shader program.
	auto camCallback = [&](const glm::mat4& data) -> void {
		r.UpdateCamUBO(data);
	};
	CameraSystem::UpdateCamView(camCallback);


	auto lightsCallback = [&](const LightVariant& light, size_t index) -> void {
		r.UpdateLightUBO(light, index);
	};
	LightSystem::UpdateLights(lightsCallback);

	// Draw models!
	auto rendererCallback = [&](std::shared_ptr<LoadedModel> model, std::shared_ptr<Shader> shader, const std::vector<glm::mat4>& data) -> void {
		r.DrawInstanced(model, shader, data);
	};

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BatchingSystem::Dispatch(*m_manager.get(), rendererCallback);
	r.SwapBuffers();
}


std::shared_ptr<CameraComponent> Scene::SetCamera(Entity e)
{
	if (m_Cameras.get()->Contains(e)) {
		m_CurCam = e;
		return CameraSystem::SetCurrent(e);
	}
	std::cout << "ERROR::CAMERA_COMPONENT_DOES_NOT_EXIST_FOR_ENTITY: " << e << std::endl;
	return nullptr;
}

void Scene::SetCamUBO(unsigned int w, unsigned int h, Renderer& r)
{
	auto projCallback = [&](const glm::mat4& proj) -> void {
		r.UpdateCamProj(proj);
	};
	CameraSystem::UpdateCamProj(w, h, projCallback);

	auto viewCallback = [&](const glm::mat4& view) -> void {
		r.UpdateCamUBO(view);
	};
	CameraSystem::UpdateCamView(viewCallback);
}

const std::vector<Entity>& Scene::GetEntityList()
{
	return m_entities;
}