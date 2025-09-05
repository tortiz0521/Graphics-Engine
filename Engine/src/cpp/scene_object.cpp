#include "../headers/scene_object.h"

Entity::Entity(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale,
	std::string_view mName, std::string_view sName)
	: m_pos(pos), m_rotation(rotation), m_scale(scale), m_model(mName), m_shader(sName) {}

const glm::vec3 Entity::GetPosition()
{
	return m_pos;
}

const glm::vec3 Entity::GetRotation()
{
	return m_rotation;
}

const glm::vec3 Entity::GetScale()
{
	return m_scale;
}

const glm::mat4 Entity::CalculateModelMat()
{
	// Calculation order: Scale -> Rotate -> Translate
	// Reverse the order because matrix calculations happen from right to left.

	glm::mat4 model = glm::mat4(1.0f);

	// Translate
	model = glm::translate(model, m_pos);

	// Rotate (Time to consider quaternions...)
	model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Scale
	model = glm::scale(model, m_scale);

	return model;
}



Light::Light(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale, std::string_view mName, std::string_view sName,
	glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec, float constant, float linear, float quad, 
	float inCut, float outCut, LightType type)
	: Entity(pos, rotation, scale, mName, sName), m_ambient(ambient), m_diffuse(diffuse), m_specular(spec),
	m_const(constant), m_linear(linear), m_quad(quad), m_inCutOff(inCut), m_outCutOff(outCut), m_type(type) {}

