#ifndef LIGHT_SYSTEM_H
#define LIGHT_SYSTEM_H

#include "../components/light_component.h"
#include <functional>

namespace LightSystem
{
	std::shared_ptr<LightComponent> AddLight(Entity e, LightType type, LightComponent&& data);

	void SortLights();

	void PopulateLightUBOs(std::function<void(const LightVariant& l)> AddUBOStruct);

	void UpdateLights(std::function<void(const LightVariant& l, size_t index)> UpdateUBO);

	const std::shared_ptr<LightComponent> GetLightStruct(Entity e);
}


#endif