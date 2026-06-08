#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "../_component_manager.h"
#include "../../resource/resource_manager.h"

typedef uint32_t modelID;
typedef uint32_t shaderID;

struct RenderComponent 
{
	modelID model{};
	shaderID shader{};

	bool* dirty{};

	RenderComponent& SetModel(uint32_t m)
	{
		model = m;
		*dirty = false;
		return *this;
	}

	RenderComponent& SetShader(uint32_t s)
	{
		shader = s;
		*dirty = false;
		return *this;
	}

	void read_model(std::string_view val)
	{
		model = AssetRegistry::path_ID[std::string(val)];
	}

	auto write_model()
	{
		return AssetRegistry::PathFromID(model);
	}

	void read_shader(std::string_view val)
	{
		shader = AssetRegistry::path_ID[std::string(val)];
	}

	auto write_shader()
	{
		return AssetRegistry::PathFromID(shader);
	}

	RenderComponent& operator=(const RenderComponent& rhs)
	{
		model = rhs.model;
		shader = rhs.shader;

		return *this;
	}
};

extern std::shared_ptr<ComponentManager<RenderComponent>> renders;


#endif // !MODEL_COMPONENT_H
