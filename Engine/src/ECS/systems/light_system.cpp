#include "light_system.h"
#include <iostream>

std::shared_ptr<LightComponent>& LightSystem::AddLight(Entity e, LightType type, LightComponent&& data,
	std::function<void(const LightVariant& l)> AddUBOStruct)
{
	std::shared_ptr<LightComponent> lc = lights.Create(e, std::move(data));
	AddUBOStruct(lc.get()->data.value);

	if (lights.GetCount() == 0)
		std::cout << "BAD_FUNC_CALL::AddLight" << std::endl;

	if (lights.GetCount() > 1) {
		for (uint32_t i = 0; i < lights.GetCount(); i++) {
			if (lc.get()->type < lights[i].get()->type) {
				lights.MoveItem(lights.GetCount() - 1, i - 1);
			}
		}
	}

	return lc;
}

void LightSystem::UpdateLights(std::function<void(const LightVariant& l, size_t index)> UpdateUBO)
{
	size_t index = 0;
	for (uint32_t i = 0; i < lights.GetCount(); i++) {
		if (true) {//lights[i].data.dirty) {
			UpdateUBO(lights[i].get()->data, index);
		}

		if (i + 1 >= lights.GetCount())
			return;

		if (lights[i + 1].get()->type > lights[i].get()->type)
			index = 0;
		else
			index++;
	}
}

const std::shared_ptr<LightComponent> LightSystem::GetLightStruct(Entity e)
{
	return lights.GetComponent(e);
}