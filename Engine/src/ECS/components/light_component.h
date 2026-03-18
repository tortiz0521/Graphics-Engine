#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "../_component_manager.h"
#include "../../headers/_ubo.h"

#include <variant>
#include <glaze/json.hpp>

enum LightType
{
	None = 0,
	Direction = 1,
	Point = 2,
	Spot = 3
};

struct LightComponent
{
	LightType type{};
	Tracker<LightVariant> data{};

	void read_data(const LightVariant& value)
	{
		if constexpr (std::is_same_v<decltype(value), DirectionLight>) {
			std::cout << "JSON_PARSE_ERROR::MISMATCH_VARIANT/LIGHT-TYPE" << std::endl;
			//return;
		}
		else if constexpr (std::is_same_v<decltype(value), PointLight>) {
			std::cout << "JSON_PARSE_ERROR::MISMATCH_VARIANT/LIGHT-TYPE" << std::endl;
			//return;
		}
		else if constexpr (std::is_same_v<decltype(value), SpotLight>) {
			std::cout << "JSON_PARSE_ERROR::MISMATCH_VARIANT/LIGHT-TYPE" << std::endl;
			//return;
		}
		std::cout << "MADE IT HERE" << std::endl;
		data = { value, &dirty };
	};

	auto& write_data()
	{
		return data.value;
	};


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

	LightComponent& operator=(const LightComponent& rhs)
	{
		type = rhs.type;
		data = { rhs.data.value, &dirty };

		return *this;
	};

private:
	bool dirty = false;
};


extern std::shared_ptr<ComponentManager<LightComponent>> lights;


#endif