#include "../headers/scene.h"
#include <algorithm>
#include <utility>

Scene::Scene()
{
	m_Cameras = std::make_unique<ComponentManager<CameraComponent>>();
	m_Hierarchies = std::make_unique<ComponentManager<HierarchyComponent>>();
	m_Lights = std::make_unique<ComponentManager<LightComponent>>();
	m_Renders = std::make_unique<ComponentManager<RenderComponent>>();
	m_Transforms = std::make_unique<ComponentManager<TransformComponent>>();

	m_manager = std::make_unique<ResourceManager>();
}

Entity Scene::AddEntity()
{
	Entity e = CreateEntity();
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
	auto rendererCallback = [&](uint32_t model, uint32_t shader, const std::vector<glm::mat4>& data) -> void {
		r.DrawInstanced(model, shader, data, m_manager.get());
	};

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BatchingSystem::Dispatch(rendererCallback);
	r.SwapBuffers();
}


std::shared_ptr<CameraComponent> Scene::SetCamera(Entity e)
{
	if (cameras.Contains(e)) {
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