#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../_component_manager.h"

struct TransformComponent 
{
	glm::mat4 localTransform;
	glm::mat4 worldTransform;
	glm::vec3 translation;
	glm::vec3 scale;
	float rotation;

	void SetTransform(glm::vec3 s = glm::vec3(1.0f), 
		glm::vec3 t = glm::vec3(1.0f), float r = 0.0f)
	{
		scale = s;
		translation = t;
		rotation = r;

		glm::mat4 m = glm::mat4(1.0f);
		m = glm::translate(m, translation);
		m = glm::rotate(m, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::scale(m, scale);
		localTransform = m;
	};

	void SetTransition(glm::vec3 s) { scale = s; };
};

ComponentManager<TransformComponent> transforms;

#endif