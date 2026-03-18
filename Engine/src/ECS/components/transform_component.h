#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../_component_manager.h"
#include <utility>

struct TransformComponent 
{
	glm::mat4 localTransform{};
	glm::mat4 worldTransform{};
	Tracker<glm::vec3> translation{};
	Tracker<glm::vec3> scale{};
	Tracker<float> rotation{};

	bool dirty = true;

	void read_translation(const std::vector<float>& val)
	{
		translation = { glm::vec3(val[0], val[1], val[2]), &dirty };
	}

	auto write_translation()
	{
		std::vector<float> val = { translation.value.x, translation.value.y, translation.value.z };
		return val;
	}

	void read_scale(const std::vector<float>& val)
	{
		scale = { glm::vec3(val[0], val[1], val[2]), &dirty };
	}

	auto write_scale()
	{
		std::vector<float> val = { scale.value.x, scale.value.y, scale.value.z };
		return val;
	}

	void read_rotation(const float val)
	{
		rotation = { val, &dirty };
	}

	auto& write_rotation()
	{
		return rotation.value;
	}

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


	TransformComponent() {};
	TransformComponent(glm::vec3 trans, glm::vec3 scl, float rot)
	{
		translation = { trans, &dirty };
		scale = { scl, &dirty };
		rotation = { rot, &dirty };
		SetTransform();
	};

	TransformComponent(TransformComponent&& tc) noexcept
	{
		translation = { tc.translation.value, &dirty };
		scale = { tc.scale.value, &dirty };
		rotation = { tc.rotation.value, &dirty };
		SetTransform();

		tc.translation = { glm::vec3(0.0f) ,&tc.dirty };
		tc.scale = { glm::vec3(0.0f) ,&tc.dirty };
		tc.rotation = { 0.0f ,&tc.dirty };
	};

	TransformComponent(const TransformComponent& tc)
	{
		translation = { tc.translation.value, &dirty };
		scale = { tc.scale.value, &dirty };
		rotation = { tc.rotation.value, &dirty };
		SetTransform();
	};

	TransformComponent& operator=(const TransformComponent& rhs)
	{
		localTransform = rhs.localTransform;
		worldTransform = rhs.worldTransform;
		translation = { rhs.translation.value, &dirty };
		scale = { rhs.scale.value, &dirty };
		rotation = { rhs.rotation.value, &dirty };

		return *this;
	}
};

extern std::shared_ptr<ComponentManager<TransformComponent>> transforms;

#endif