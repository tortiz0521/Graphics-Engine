#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H
#pragma once

#include "../_component_manager.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct TransformComponent 
{
	glm::mat4 localTransform;
	glm::mat4 worldTransform;
	glm::vec3 translation;
	glm::vec3 scale;
	float rotation;

	void SetTransforms(glm::vec3 s = glm::vec3(1.0f), 
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
};

ComponentManager<TransformComponent> transforms;



#endif