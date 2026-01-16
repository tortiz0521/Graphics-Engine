#include "../headers/scene_object.h"

Entity::Entity(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale,
	uint32_t modelID, uint32_t shaderID)
	: m_pos(pos), m_rotation(rotation), m_scale(scale), m_model(modelID), m_shader(shaderID) {}

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

const glm::mat4 Entity::CalculateModelMat(glm::mat4& model)
{
	// Calculation order: Scale -> Rotate -> Translate
	// Reverse the order because matrix calculations happen from right to left.

	// Translate
	model = glm::translate(model, m_pos);

	// Rotate (Time to consider quaternions...)
	model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Scale
	model = glm::scale(model, m_scale);

	return model;
}


void Entity::AddChild(Entity& c)
{
	m_children.emplace_back(std::make_unique<Entity>(c));
}

const std::vector<std::unique_ptr<Entity>> Entity::GetChildren()
{
	return m_children;
}


Light::Light(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale, uint32_t modelID, uint32_t shaderID,
	glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec, float constant, float linear, float quad, 
	float inCut, float outCut, LightType type)
	: Entity(pos, rotation, scale, modelID, shaderID), m_ambient(ambient), m_diffuse(diffuse), m_specular(spec),
	m_const(constant), m_linear(linear), m_quad(quad), m_inCutOff(inCut), m_outCutOff(outCut), m_type(type) {}

