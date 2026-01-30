#include "hierarchy_system.h"

// When we update our object transforms, the parent is the primary object that gets effected.
// Each child object will be updated by multiplying the local transform (parent space) to the
// updated world transform of the parent. This will give us the child's world space coordinates.

// Attach a new entity to the hierarchy system.
void HierarchySystem::Attach(Entity entity, Entity parent)
{
	HierarchyComponent& hc = hierarchy.Create(entity);
	hc.parent = parent;

	// Now, search if the 'child' is a 'parent' of any other entities and swap them!
	if (hierarchy.GetCount() > 1) {
		for (size_t i = hierarchy.GetCount(); i > 0; i--) {
			for (size_t j = 0; j < i; j++) {
				if (hierarchy[j].parent == hierarchy.GetEntity(i)) {
					hierarchy.MoveItem(i, j);
					transforms.MoveItem(i, j);
					++i;
				}
			}
		}
	}

	// Make sure that the necessary hierarchy components exist.
	TransformComponent* parentTransform = transforms.GetComponent(parent);
	if (parentTransform == nullptr) {
		transforms.Create(parent).SetTransform();
	}
	hc.parentInverseBind = glm::inverse(parentTransform->worldTransform);
}

// Detach an entity from the hierarchy system.
void HierarchySystem::Detach(Entity e)
{
	HierarchyComponent* hc = hierarchy.GetComponent(e);

	if (hc != nullptr) {
		TransformComponent* tc = transforms.GetComponent(e);
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
	HierarchyComponent* hc = hierarchy.GetComponent(e);

	if (hc != nullptr && hierarchy.GetComponent(newParent) != nullptr) {
		TransformComponent* tc = transforms.GetComponent(e);
		TransformComponent* ptc = transforms.GetComponent(newParent);

		if (tc != nullptr && ptc != nullptr) {
			tc->localTransform = glm::inverse(ptc->worldTransform) * tc->worldTransform;
		}

		hc->parent = newParent;
	}
}

void HierarchySystem::UpdateHierarchySystem()
{
	transforms[0].worldTransform = transforms[0].localTransform;
	glm::mat4 prev = transforms[0].worldTransform;
	for (uint32_t i = 1; i < transforms.GetCount(); i++) {
		prev = transforms[i].worldTransform;

		if (hierarchy[i].parent == INVALID_ENTITY) {
			transforms[i].worldTransform = transforms[i].localTransform;
			continue;
		}
		
		transforms[i].worldTransform = prev * transforms[i].localTransform;
	}
}