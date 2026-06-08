#include "hierarchy_system.h"
#include <iostream>

// When we update our object transforms, the parent is the primary object that gets effected.
// Each child object will be updated by multiplying the local transform (parent space) to the
// updated world transform of the parent. This will give us the child's world space coordinates.

// Attach a new entity to the hierarchy system.
std::shared_ptr<HierarchyComponent> HierarchySystem::Attach(Entity entity, Entity parent)
{
	std::shared_ptr<HierarchyComponent> hc = hierarchy.get()->Create(entity);
	hc.get()->parent = parent;

	containerUpdate(hc.get());
	return hc;
}

std::shared_ptr<HierarchyComponent> HierarchySystem::Attach(Entity e, HierarchyComponent&& comp)
{
	std::shared_ptr<HierarchyComponent> hc = hierarchy.get()->Create(e, std::move(comp));
	containerUpdate(hc.get());
	return hc;
}

// Detach an entity from the hierarchy system.
void HierarchySystem::Detach(Entity e)
{
	std::shared_ptr<HierarchyComponent> hc = hierarchy.get()->GetComponent(e);

	if (hc != nullptr) {
		std::shared_ptr<TransformComponent> tc = transforms.get()->GetComponent(e);
		if (tc != nullptr) {
			tc->localTransform = tc->worldTransform;
		}

		hc->parent = INVALID_ENTITY;
		size_t index = hierarchy.get()->GetIndex(e);
		hierarchy.get()->MoveItem(index, hierarchy.get()->GetCount());
	} 
}

void HierarchySystem::Reparent(Entity e, Entity newParent)
{
	std::shared_ptr<HierarchyComponent> hc = hierarchy.get()->GetComponent(e);

	if (hc != nullptr && hierarchy.get()->GetComponent(newParent) != nullptr) {
		std::shared_ptr<TransformComponent> tc = transforms.get()->GetComponent(e);
		std::shared_ptr<TransformComponent> ptc = transforms.get()->GetComponent(newParent);

		if (tc != nullptr && ptc != nullptr) {
			tc->localTransform = glm::inverse(ptc->worldTransform) * tc->worldTransform;
		}

		hc->parent = newParent;
	}
}

void HierarchySystem::ResetHierarchy()
{
	containerUpdate();
}

void HierarchySystem::UpdateHierarchySystem()
{
	std::shared_ptr<LightComponent> lc = lights.get()->GetComponent(transforms.get()->GetEntity(0));
	if ((*transforms)[0].get()->dirty) {
		(*transforms)[0].get()->SetTransform();
		(*transforms)[0].get()->worldTransform = (*transforms)[0].get()->localTransform;

		if (lc) {
			switch (lc->data.value.index()) {
				case 0: break;
				case 1:
					std::get<PointLight>(lc->data.value).pos = glm::vec4(
						(*transforms.get())[0].get()->worldTransform[0][3],
						(*transforms.get())[0].get()->worldTransform[1][3],
						(*transforms.get())[0].get()->worldTransform[2][3],
						1.0f
					);
					break;
				case 2:
					std::get<SpotLight>(lc->data.value).pos = glm::vec4(
						(*transforms.get())[0].get()->worldTransform[0][3],
						(*transforms.get())[0].get()->worldTransform[1][3],
						(*transforms.get())[0].get()->worldTransform[2][3],
						1.0f
					);
			}
		}

		(*transforms.get())[0].get()->dirty = false;
	}

	uint32_t curParent = INVALID_ENTITY;
	glm::mat4 prev{};
	for (uint32_t i = 1; i < transforms.get()->GetCount(); i++) {
		if ((*hierarchy)[i]->parent != curParent) {
			curParent = (*hierarchy)[i]->parent;
		}

		if ((*transforms)[i].get()->dirty) {
			(*transforms)[i].get()->SetTransform();
			(*transforms)[i].get()->dirty = false;
		}

		if ((*hierarchy)[i].get()->parent == INVALID_ENTITY) {
			(*transforms)[i].get()->worldTransform = (*transforms)[i].get()->localTransform;
			continue;
		}
		
		(*transforms)[i].get()->worldTransform = 
			(*transforms)[transforms->GetIndex(curParent)].get()->worldTransform * (*transforms)[i].get()->localTransform;

		lc = lights->GetComponent(transforms->GetEntity(i));
		if (lc) {
			switch (lc->data.value.index()) {
				case 0: break;
				case 1:
					std::get<PointLight>(lc->data.value).pos = glm::vec4(
						(*transforms)[0].get()->worldTransform[0][3],
						(*transforms)[0].get()->worldTransform[1][3],
						(*transforms)[0].get()->worldTransform[2][3],
						1.0f
					);
					break;
				case 2:
					std::get<SpotLight>(lc->data.value).pos = glm::vec4(
						(*transforms)[0].get()->worldTransform[0][3],
						(*transforms)[0].get()->worldTransform[1][3],
						(*transforms)[0].get()->worldTransform[2][3],
						1.0f
					);
			}
		}
	}
}