#ifndef SCENE_H
#define SCENE_H
#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include "../renderer/renderer.h"
#include "../ECS/_component_manager.h"
#include "../ECS/systems/batching_system.h"
#include "../ECS/systems/camera_system.h"
#include "../ECS/systems/hierarchy_system.h"
#include "../ECS/systems/light_system.h"

template<typename Type> 
concept Component = (std::is_same_v<Type, CameraComponent> ||
	std::is_same_v<Type, HierarchyComponent> ||
	std::is_same_v<Type, LightComponent> ||
	std::is_same_v<Type, RenderComponent> ||
	std::is_same_v<Type, TransformComponent>);

class Scene 
{
public:
	Scene();
	//Scene(Some JSON object...);
	void Unload();
	void Load(ResourceManager& rm, Renderer& r);
	void RenderScene(Renderer& r);

	Entity AddEntity();
	template<Component T>
	std::shared_ptr<T> AddComponent(Entity e, T&& data, LightType lightType = None);
	template<Component T>
	std::shared_ptr<T> AddComponent(Entity e);
	template<Component T>
	std::shared_ptr<T> GetComponent(Entity e);

	std::shared_ptr<CameraComponent> SetCamera(Entity e);
	void SetCamUBO(unsigned int w, unsigned int h, Renderer& r);

	std::unique_ptr<SceneResourceManager> m_manager;

	const std::vector<Entity>& GetEntityList();
	bool RemoveEntity(Entity e);

private:
	// Entity list
	std::vector<Entity> m_entities{};
	std::shared_ptr<ComponentManager<CameraComponent>> m_Cameras{};
	std::shared_ptr<ComponentManager<HierarchyComponent>> m_Hierarchies{};
	std::shared_ptr<ComponentManager<LightComponent>> m_Lights{};
	std::shared_ptr<ComponentManager<RenderComponent>> m_Renders{};
	std::shared_ptr<ComponentManager<TransformComponent>> m_Transforms{};

	Entity m_CurCam = INVALID_ENTITY;

	bool m_uLight = 0, m_uHier = 0, m_uBatch = 0;
};

template<Component T>
std::shared_ptr<T> Scene::AddComponent(Entity e, T&& data, LightType lightType)
{
	auto comp = [&](auto&& x) -> auto {
		if constexpr (std::is_same_v<std::decay_t<decltype(x)>, CameraComponent>) {
			if (std::shared_ptr<CameraComponent> c = m_Cameras.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<CameraComponent>" << std::endl;
				return c;
			}

			return m_Cameras.get()->Create(e, static_cast<CameraComponent>(x));
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, HierarchyComponent>) {
			if (std::shared_ptr<HierarchyComponent> c = m_Hierarchies.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<HierarchyComponent>" << std::endl;
				return c;
			}

			if (hierarchy.get() == m_Hierarchies.get())
				return HierarchySystem::Attach(e, static_cast<HierarchyComponent>(x));
			else {
				std::shared_ptr<ComponentManager<HierarchyComponent>> temp = hierarchy;
				hierarchy = m_Hierarchies;
				auto comp = HierarchySystem::Attach(e, static_cast<HierarchyComponent>(x));
				hierarchy = temp;
				return comp;
			}
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, LightComponent>) {
			if (std::shared_ptr<LightComponent> c = m_Lights.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<LightComponent>" << std::endl;
				return c;
			}

			if (lightType != 0) {
				m_uLight = true;
				if (lights.get() == m_Lights.get())
					return LightSystem::AddLight(e, lightType, static_cast<LightComponent>(x));
				else {
					std::cout << "ADD_COMPONENT_ERROR::CONTAINER_NOT_SET_TO_ACTIVE_SCENE" << std::endl;
					return m_Lights.get()->Create(e, static_cast<LightComponent>(x));
				}
			}
			else {
				std::cout << "ADD_COMPONENT_ERROR::NEED_LIGHT_TYPE_FOR_STRUCT_INSTANTIATION::<LightComponent>" << std::endl;
				//return nullptr;
			}
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, RenderComponent>) {
			if (std::shared_ptr<RenderComponent> c = m_Renders.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<RenderComponent>" << std::endl;
				return c;
			}

			if (renders.get() == m_Renders.get()) {
				std::shared_ptr<RenderComponent> rc = m_Renders.get()->Create(e, static_cast<RenderComponent>(x));
				BatchingSystem::Batch();
				return rc;
			}
			else {
				m_uBatch = true;
				std::cout << "ADD_COMPONENT_ERROR::CONTAINER_NOT_SET_TO_ACTIVE_SCENE" << std::endl;
				std::shared_ptr<RenderComponent> rc = m_Renders.get()->Create(e, static_cast<RenderComponent>(x));
				rc.get()->dirty = m_Renders.get()->GetDirty();
				return rc;
			}
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, TransformComponent>) {
			if (std::shared_ptr<TransformComponent> c = m_Transforms.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<TransformComponent>" << std::endl;
				return c;
			}

			std::shared_ptr<TransformComponent> tc = m_Transforms.get()->Create(e, static_cast<TransformComponent>(x));
			if (m_Hierarchies->Contains(e)) {
				m_Transforms->MoveItem(m_Transforms->GetIndex(e), m_Hierarchies->GetIndex(e));
			}
			else {
				m_uHier = true;
				//m_Hierarchies->Create(e);
			}
			return tc;
		}
		else {
			std::cout << "ADD_COMPONENT_ERROR::COMPONENT_TYPE_IS_NOT_AVAILABLE" << std::endl;
			//return nullptr;
		}
	};

	return comp(data);
}

template<Component T>
std::shared_ptr<T> Scene::AddComponent(Entity e)
{
	auto comp = [&]() {
		if constexpr (std::is_same_v<std::decay_t<T>, CameraComponent>) {
			if (std::shared_ptr<CameraComponent> c = m_Cameras.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<CameraComponent>" << std::endl;
				return c;
			}

			return m_Cameras.get()->Create(e);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, HierarchyComponent>) {
			if (std::shared_ptr<HierarchyComponent> c = m_Hierarchies.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<HierarchyComponent>" << std::endl;
				return c;
			}

			std::shared_ptr<HierarchyComponent> temp = m_Hierarchies.get()->Create(e);
			HierarchySystem::Attach(e, INVALID_ENTITY);
				
			return temp;
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, LightComponent>) {
			if (std::shared_ptr<LightComponent> c = m_Lights.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<LightComponent>" << std::endl;
				return c;
			}

			if (lights.get() == m_Lights.get())
				return m_Lights.get()->Create(e);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, RenderComponent>) {
			if (std::shared_ptr<RenderComponent> c = m_Renders.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<RenderComponent>" << std::endl;
				return c;
			}
			if (renders.get() == m_Renders.get())
				return m_Renders.get()->Create(e);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, TransformComponent>) {
			if (std::shared_ptr<TransformComponent> c = m_Transforms.get()->GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<TransformComponent>" << std::endl;
				return c;
			}

			if (transforms.get() == m_Transforms.get())
				return m_Transforms.get()->Create(e);
		}
		else { 
			return nullptr;
		}
	};

	//if (!comp)
		//std::cout << "SOMETHING WENT WRONG." << std::endl;
	return comp();
}

template<Component T>
std::shared_ptr<T> Scene::GetComponent(Entity e)
{
	auto comp = [&]() -> auto {
		if constexpr (std::is_same_v<std::decay_t<T>, CameraComponent>) { return m_Cameras.get()->GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, HierarchyComponent>) { return m_Hierarchies.get()->GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, LightComponent>) { return m_Lights.get()->GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, RenderComponent>) { return m_Renders.get()->GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, TransformComponent>) { return m_Transforms.get()->GetComponent(e); }
		else { //return nullptr;
		}
	};

	//if (!comp)
		//std::cout << "SOMETHING WENT WRONG." << std::endl;
	return comp();
}


#endif