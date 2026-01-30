#ifndef HIERARCHY_SYSTEM_H
#define HIERARCHY_SYSTEM_H

#include "../components/hierarchy_component.h"
#include "../components/transform_component.h"

#include <glm/glm.hpp>

// This system updates between frames! The goal is just to organize data properly. Preparing the data for render will be the job of the rendering system.

namespace HierarchySystem {
	// Anonymous namespace: Limit public access within a file. Only functions within the translation unit have access to these.
	namespace {
		void ApplyTransform(TransformComponent* parent, TransformComponent* child) {
			child->worldTransform = parent->localTransform * child->localTransform;
		};
	}

	// Hierarchy attachment funcs
	void Attach(Entity entity, Entity parent);
	void Detach(Entity e);
	void Reparent(Entity child, Entity parent);

	void UpdateHierarchySystem();
};


#endif