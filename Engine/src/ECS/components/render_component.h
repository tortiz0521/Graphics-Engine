#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "../_component_manager.h"

typedef uint32_t modelID;
typedef uint32_t shaderID;

struct RenderComponent 
{
	modelID model{};
	shaderID shader{};
};

ComponentManager<RenderComponent> renders{};


#endif // !MODEL_COMPONENT_H
