#ifndef HIERARCHY_SYSTEM_H
#define HIERARCHY_SYSTEM_H

#include "../components/hierarchy_component.h"
#include "../components/transform_component.h"

namespace HierarchySystem {
	// Anonymous namespace: Limit public access within a file. Only functions within the translation unit have access to these.
	namespace {
		void ApplyTransform(TransformComponent* parent, TransformComponent* child) {
			child->worldTransform = parent->localTransform * child->localTransform;
		};

		glm::mat4 GiveParent(HierarchyComponent& child, Entity parent) {

			//child->parentInverseBind =
		};
	}

	// Hierarchy attachment funcs
	void Attach(Entity entity, Entity parent);
	void Detach(Entity e);
	void Reparent(Entity child, Entity parent);

	void UpdateHierarchySystem();
};


#endif