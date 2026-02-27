#include "hierarchy_system.h"
#include <iostream>

// When we update our object transforms, the parent is the primary object that gets effected.
// Each child object will be updated by multiplying the local transform (parent space) to the
// updated world transform of the parent. This will give us the child's world space coordinates.

// Attach a new entity to the hierarchy system.
std::shared_ptr<HierarchyComponent>& HierarchySystem::Attach(Entity entity, Entity parent)
{
	std::shared_ptr<HierarchyComponent>& hc = hierarchy.Create(entity);
	hc.get()->parent = parent;

	containerUpdate(*hc.get());
	return hc;
}

std::shared_ptr<HierarchyComponent>& HierarchySystem::Attach(Entity e, HierarchyComponent&& comp)
{
	std::shared_ptr<HierarchyComponent>& hc = hierarchy.Create(e, std::move(comp));
	containerUpdate(*hc.get());
	return hc;
}

// Detach an entity from the hierarchy system.
void HierarchySystem::Detach(Entity e)
{
	std::shared_ptr<HierarchyComponent> hc = hierarchy.GetComponent(e);

	if (hc != nullptr) {
		std::shared_ptr<TransformComponent> tc = transforms.GetComponent(e);
		if (tc != nullptr) {
			tc->localTransform = tc->worldTransform;
		}

		hc->parent = INVALID_ENTITY;
		size_t index = hierarchy.GetIndex(e);
		hierarchy.MoveItem(index, hierarchy.GetCount());
	} 
}

void HierarchySystem::Reparent(Entity e, Entity newParent)
{
	std::shared_ptr<HierarchyComponent> hc = hierarchy.GetComponent(e);

	if (hc != nullptr && hierarchy.GetComponent(newParent) != nullptr) {
		std::shared_ptr<TransformComponent> tc = transforms.GetComponent(e);
		std::shared_ptr<TransformComponent> ptc = transforms.GetComponent(newParent);

		if (tc != nullptr && ptc != nullptr) {
			tc->localTransform = glm::inverse(ptc->worldTransform) * tc->worldTransform;
		}

		hc->parent = newParent;
	}
}

void HierarchySystem::UpdateHierarchySystem()
{
	std::shared_ptr<LightComponent> lc = lights.GetComponent(transforms.GetEntity(0));
	if (transforms[0].get()->dirty) {
		transforms[0].get()->SetTransform();
		transforms[0].get()->worldTransform = transforms[0].get()->localTransform;

		if (lc) {
			switch (lc->data.value.index()) {
				case 0: break;
				case 1:
					std::get<PointLight>(lc->data.value).pos = glm::vec4(
						transforms[0].get()->worldTransform[0][3],
						transforms[0].get()->worldTransform[1][3],
						transforms[0].get()->worldTransform[2][3],
						1.0f
					);
					break;
				case 2:
					std::get<SpotLight>(lc->data.value).pos = glm::vec4(
						transforms[0].get()->worldTransform[0][3],
						transforms[0].get()->worldTransform[1][3],
						transforms[0].get()->worldTransform[2][3],
						1.0f
					);
			}
		}

		transforms[0].get()->dirty = false;
	}

	glm::mat4 prev = transforms[0].get()->worldTransform;
	for (uint32_t i = 1; i < transforms.GetCount(); i++) {
		if (transforms[i].get()->dirty) {
			transforms[i].get()->SetTransform();
			transforms[i].get()->dirty = false;
		}

		if (hierarchy[i].get()->parent == INVALID_ENTITY) {
			transforms[i].get()->worldTransform = transforms[i].get()->localTransform;
			continue;
		}
		
		transforms[i].get()->worldTransform = prev * transforms[i].get()->localTransform;

		lc = lights.GetComponent(transforms.GetEntity(i));
		if (lc) {
			switch (lc->data.value.index()) {
				case 0: break;
				case 1:
					std::get<PointLight>(lc->data.value).pos = glm::vec4(
						transforms[0].get()->worldTransform[0][3],
						transforms[0].get()->worldTransform[1][3],
						transforms[0].get()->worldTransform[2][3],
						1.0f
					);
					break;
				case 2:
					std::get<SpotLight>(lc->data.value).pos = glm::vec4(
						transforms[0].get()->worldTransform[0][3],
						transforms[0].get()->worldTransform[1][3],
						transforms[0].get()->worldTransform[2][3],
						1.0f
					);
			}
		}

		prev = transforms[i].get()->worldTransform;
	}
}