#ifndef SCENE_H
#define SCENE_H
#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include "renderer.h"
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
	//Scene(const ResourceManager& rm);
	//Scene(Some JSON object...);
	void RenderScene(Renderer& r);

	Entity AddEntity();

	template<Component T>
	std::shared_ptr<T> AddComponent(Entity e, Renderer& r, const T&& data, LightType lightType = None);
	template<Component T>
	std::shared_ptr<T> AddComponent(Entity e, Renderer& r);
	template<Component T>
	T& GetComponent(Entity e);

	std::shared_ptr<CameraComponent> SetCamera(Entity e);
	void SetCamUBO(unsigned int w, unsigned int h, Renderer& r);

	std::unique_ptr<ResourceManager> m_manager;

private:
	// Entity list
	std::vector<Entity> m_entities;
	std::unique_ptr<ComponentManager<CameraComponent>> m_Cameras;
	std::unique_ptr<ComponentManager<HierarchyComponent>> m_Hierarchies;
	std::unique_ptr<ComponentManager<LightComponent>> m_Lights;
	std::unique_ptr<ComponentManager<RenderComponent>> m_Renders;
	std::unique_ptr<ComponentManager<TransformComponent>> m_Transforms;

	Entity m_CurCam = INVALID_ENTITY;
};

template<Component T>
std::shared_ptr<T> Scene::AddComponent(Entity e, Renderer& r, const T&& data, LightType lightType)
{
	auto comp = [&](auto&& x) {
		if constexpr (std::is_same_v<std::decay_t<decltype(x)>, CameraComponent>) {
			if (std::shared_ptr<CameraComponent> c = cameras.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<CameraComponent>" << std::endl;
				return c;
			}

			return cameras.Create(e, static_cast<CameraComponent>(x));
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, HierarchyComponent>) {
			if (std::shared_ptr<HierarchyComponent> c = hierarchy.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<HierarchyComponent>" << std::endl;
				return c;
			}
			return HierarchySystem::Attach(3, 4);
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, LightComponent>) {
			if (std::shared_ptr<LightComponent> c = lights.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<LightComponent>" << std::endl;
				return c;
			}

			if (lightType != 0) {
				auto callback = [&](const LightVariant& l) -> void {
					r.AddUBOStruct(l);
				};
				return LightSystem::AddLight(e, lightType, static_cast<LightComponent>(x), callback);
			}
			else {
				std::cout << "ADD_COMPONENT_ERROR::NEED_LIGHT_TYPE_FOR_STRUCT_INSTANTIATION::<LightComponent>" << std::endl;
				//return nullptr;
			}
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, RenderComponent>) {
			if (std::shared_ptr<RenderComponent> c = renders.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<RenderComponent>" << std::endl;
				return c;
			}

			std::shared_ptr<RenderComponent> rc = renders.Create(e, static_cast<RenderComponent>(x));
			BatchingSystem::Batch();
			return rc;
		}
		else if constexpr (std::is_same_v<std::decay_t<decltype(x)>, TransformComponent>) {
			if (std::shared_ptr<TransformComponent> c = transforms.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<TransformComponent>" << std::endl;
				return c;
			}
			return transforms.Create(e, static_cast<TransformComponent>(x));
		}
		else {
			std::cout << "ADD_COMPONENT_ERROR::COMPONENT_TYPE_IS_NOT_AVAILABLE" << std::endl;
			//return nullptr;
		}
	};

	return comp(data);
}

template<Component T>
std::shared_ptr<T> Scene::AddComponent(Entity e, Renderer& r)
{
	auto comp = [&]() {
		if constexpr (std::is_same_v<std::decay_t<T>, CameraComponent>) {
			if (std::shared_ptr<CameraComponent> c = cameras.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<CameraComponent>" << std::endl;
				return c;
			}
			return cameras.Create(e);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, HierarchyComponent>) {
			if (std::shared_ptr<HierarchyComponent> c = hierarchy.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<HierarchyComponent>" << std::endl;
				return c;
			}
			return hierarchy.Create(e);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, LightComponent>) {
			if (std::shared_ptr<LightComponent> c = transforms.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<LightComponent>" << std::endl;
				return c;
			}
			return lights.Create(e);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, RenderComponent>) {
			if (std::shared_ptr<RenderComponent> c = transforms.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<RenderComponent>" << std::endl;
				return c;
			}
			return renders.Create(e);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, TransformComponent>) {
			if (std::shared_ptr<TransformComponent> c = transforms.GetComponent(e)) {
				std::cout << "ADD_COMPONENT_ERROR::COMPONENT_ALREADY_EXISTS::<TransformComponent>" << std::endl;
				return c;
			}
			return transforms.Create(e);
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
T& Scene::GetComponent(Entity e)
{
	auto comp = [&]() -> auto* {
		if constexpr (std::is_same_v<std::decay_t<T>, CameraComponent>) { return cameras.GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, HierarchyComponent>) { return hierarchy.GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, LightComponent>) { return lights.GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, RenderComponent>) { return renders.GetComponent(e); }
		else if constexpr (std::is_same_v<std::decay_t<T>, TransformComponent>) { return transforms.GetComponent(e); }
		else { //return nullptr;
		}
	};

	//if (!comp)
		//std::cout << "SOMETHING WENT WRONG." << std::endl;
	return *comp();
}


#endif