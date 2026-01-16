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
					++i;
				}
			}
		}
	}

	// Make sure that the necessary hierarchy components are created.
	TransformComponent* parentTransform = transforms.GetComponent(parent);
	if (parentTransform == nullptr) {
		transforms.Create(parent).SetTransforms();
	}
	hc.parentInverseBind = glm::inverse(parentTransform->worldTransform);
}

// Detach an entity from the hierarchy system.
void HierarchySystem::Detach(Entity e)
{
	const HierarchyComponent* hc = hierarchy.GetComponent(e);

	if (hc != nullptr) {
		TransformComponent* tc = transforms.GetComponent(e);
		if (tc != nullptr) {
			tc->localTransform = tc->worldTransform;
		}

		hierarchy.RemoveKeepSorted(e);

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