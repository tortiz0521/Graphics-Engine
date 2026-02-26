#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../_component_manager.h"

struct TransformComponent 
{
	bool dirty = true;

	glm::mat4 localTransform{};
	glm::mat4 worldTransform{};
	Tracker<glm::vec3> translation{};
	Tracker<glm::vec3> scale{};
	Tracker<float> rotation{};

	TransformComponent() {};
	TransformComponent(glm::vec3 trans, glm::vec3 scl, float rot)
	{
		translation = { trans, &dirty };
		scale = { scl, &dirty };
		rotation = { rot, &dirty };
		SetTransform();
	};

	void SetTransform(glm::vec3 s, 
		glm::vec3 t, float r)
	{
		scale = s;
		translation = t;
		rotation = r;

		glm::mat4 m = glm::mat4(1.0f);
		m = glm::translate(m, translation.value);
		m = glm::rotate(m, glm::radians(rotation.value), glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::scale(m, scale.value);
		localTransform = m;
	};

	void SetTransform()
	{
		glm::mat4 m = glm::mat4(1.0f);
		m = glm::translate(m, translation.value);
		m = glm::rotate(m, glm::radians(rotation.value), glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::scale(m, scale.value);
		localTransform = m;
	}
};

extern ComponentManager<TransformComponent> transforms;

#endif