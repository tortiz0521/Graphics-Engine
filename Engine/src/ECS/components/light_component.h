#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "../_component_manager.h"
#include "../../headers/_ubo.h"

#include <variant>

enum LightType
{
	None = 0,
	Direction,
	Point,
	Spot
};

struct LightComponent
{
	LightType type{};
	Tracker<LightVariant> data{};

	LightComponent(LightType t, LightVariant l)
	{
		type = t;
		data = { l, &dirty };
	};

	LightComponent(LightComponent&& lc) noexcept
	{
		type = lc.type;
		data = { lc.data, &dirty };

		lc.data = { LightVariant{}, &lc.dirty };
	};

	LightComponent(const LightComponent& lc)
	{
		type = lc.type;
		data = { lc.data, &dirty };
	};

	LightComponent() {};

private:
	bool dirty = false;
};


extern ComponentManager<LightComponent> lights;


#endif