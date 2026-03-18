#include "light_system.h"
#include <iostream>

std::shared_ptr<LightComponent> LightSystem::AddLight(Entity e, LightType type, LightComponent&& data)
{
	std::shared_ptr<LightComponent> lc = lights.get()->Create(e, std::move(data));

	if (lights.get()->GetCount() == 0)
		std::cout << "BAD_FUNC_CALL::AddLight" << std::endl;

	if (lights.get()->GetCount() > 1) {
		for (uint32_t i = 0; i < lights.get()->GetCount(); i++) {
			if (lc.get()->type < (*lights.get())[i].get()->type) {
				lights.get()->MoveItem(lights.get()->GetCount() - 1, i - 1);
			}
		}
	}

	return lc;
}

void LightSystem::SortLights()
{
	std::vector<std::shared_ptr<LightComponent>> lbubble{};
	std::vector<Entity> ebubble{};

	for (int i = 0; i < lights.get()->GetCount(); i++) {
		lbubble.emplace_back((*lights.get())[i]);
		ebubble.emplace_back(lights.get()->GetEntity(i));
		
		if (lbubble.size() == 0)
			continue;

		for (int j = lbubble.size() - 1; j > 0; j--) {
			if (lbubble[j - 1].get()->type > lbubble[j].get()->type) {
				std::swap(lbubble[j], lbubble[j - 1]);
				std::swap(ebubble[j], ebubble[j - 1]);
			}
			else break;
		}
	}

	lights.get()->UpdateContainers(std::move(lbubble), std::move(ebubble));
}

void LightSystem::PopulateLightUBOs(std::function<void(const LightVariant& l)> AddUBOStruct)
{
	for (int i = 0; i < lights.get()->GetCount(); i++) {
		AddUBOStruct((*lights.get())[i].get()->data.value);
	}
}

void LightSystem::UpdateLights(std::function<void(const LightVariant& l, size_t index)> UpdateUBO)
{
	size_t index = 0;
	for (uint32_t i = 0; i < lights.get()->GetCount(); i++) {
		if ((*lights.get())[i].get()->data.dirty) {
			UpdateUBO((*lights.get())[i].get()->data, index);
		}

		if (i + 1 >= lights.get()->GetCount())
			return;

		if ((*lights.get())[i + 1].get()->type > (*lights.get())[i].get()->type)
			index = 0;
		else
			index++;
	}
}

const std::shared_ptr<LightComponent> LightSystem::GetLightStruct(Entity e)
{
	return lights.get()->GetComponent(e);
}