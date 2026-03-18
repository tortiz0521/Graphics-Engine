#ifndef HIERARCHY_SYSTEM_H
#define HIERARCHY_SYSTEM_H

// Necessary for hierarchy
#include "../components/hierarchy_component.h"
#include "../components/transform_component.h"
// Needed for light updates on position changes.
#include "../components/light_component.h"

//#include <glm/glm.hpp>

// This system updates between frames! The goal is just to organize data properly. Preparing the data for render will be the job of the rendering system.

namespace HierarchySystem {
	// Anonymous namespace: Limit public access within a file. Only functions within the translation unit have access to these.
	namespace {
		void ApplyTransform(TransformComponent* parent, TransformComponent* child) 
		{
			child->worldTransform = parent->localTransform * child->localTransform;
		};

		void containerUpdate(HierarchyComponent* hc = nullptr)
		{
			// Search if the 'child' is a 'parent' of any other entities and swap them!
			if (hierarchy.get()->GetCount() > 1) {
				for (size_t i = hierarchy.get()->GetCount() - 1; i > 0; i--) {
					for (size_t j = 0; j < i; j++) {
						if ((*hierarchy.get())[j].get()->parent == hierarchy.get()->GetEntity(i)) {
							hierarchy.get()->MoveItem(i, j);
							transforms.get()->MoveItem(i, j);
							++i;
						}
					}
				}
			}

			// Make sure that the necessary hierarchy components exists for the parent.
			if (!hc)
				return;
			else if (hc->parent == INVALID_ENTITY)
				return;

			std::shared_ptr<TransformComponent> parentTransform = transforms.get()->GetComponent(hc->parent);
			if (parentTransform == nullptr) {
				transforms.get()->Create(hc->parent);
					//.SetTransform();
			}
			else {
				hc->parentInverseBind = glm::inverse(parentTransform->worldTransform);
			}
		};
	}

	// Hierarchy attachment funcs
	std::shared_ptr<HierarchyComponent>& Attach(Entity entity, Entity parent);
	std::shared_ptr<HierarchyComponent>& Attach(Entity e, HierarchyComponent&& hc);
	void Detach(Entity e);
	void Reparent(Entity child, Entity parent);

	void ResetHierarchy();
	void UpdateHierarchySystem();
};


#endif