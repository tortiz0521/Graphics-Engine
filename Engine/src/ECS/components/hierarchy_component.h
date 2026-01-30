#ifndef HIERARCHY_COMPONENT_H
#define HIERARCHY_COMPONENT_H
#pragma once

#include "../_component_manager.h"
// Include any hierarchy dependant components!


// This component doesn't need to know about the children it has! 
struct HierarchyComponent
{
	Entity parent = INVALID_ENTITY;
	glm::mat4 parentInverseBind{}; // For skeletons on deparenting/reparenting!
};

// This exists here because there should only be ONE manager!
ComponentManager<HierarchyComponent> hierarchy{};

#endif