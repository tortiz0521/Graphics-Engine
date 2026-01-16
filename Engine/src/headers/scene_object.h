#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <string_view>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

/*
	// ENTITY OBJECT //
	An entity is any object that exists within a scene. An entity has these basic qualities:
		- Position
		- Rotation
		- Scale

	In an entity tree system, each entity will also have:
		- Children Entities.

	An entity also needs to give their model matrix when it is time to render.
*/
class Entity
{
public:
	Entity(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale,
		uint32_t modelID, uint32_t shaderID);

	// Getters for entity state vars:
	const glm::vec3 GetPosition();
	const glm::vec3 GetRotation();
	const glm::vec3 GetScale();

	void UpdateEntity();
	const glm::mat4 CalculateModelMat(glm::mat4& model);

	void AddChild(Entity& c);
	const std::vector<std::unique_ptr<Entity>> GetChildren();

private:
	uint32_t m_model, m_shader;

	// State vars:
	glm::vec3 m_pos, m_rotation, m_scale;

	// List of child entities for general entity system.
	std::vector<std::unique_ptr<Entity>> m_children;
};


// LIGHT: Inherits Entity, defines extra vars for the type of light it is.
/*enum LightType {
	DIRECTION,
	POINT,
	SPOT
};

class Light : Entity
{
public:
	Light(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale, uint32_t modelID, uint32_t shaderID,
		glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec, float constant, float linear, 
		float quad, float inCut, float outCut, LightType type);

	// Light getters:

private:
	// General light components (in a vec for potentially better effects based on color).
	glm::vec3 m_ambient, m_diffuse, m_specular;

	// Attenuation values (primarily for points/spots).
	float m_const, m_linear, m_quad;
	// Allow us to control the edges of a spot light.
	float m_inCutOff, m_outCutOff;

	LightType m_type;
};

// Why create a light like as an entity? Light sources SHOULD have some sort of object associated
// with them (the exception being a directional light). Making a light specifically inherited from
// an entity allows us to have some sort of entity that will ALSO emit light.


enum CAM_MOVEMENT {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera : Entity
{
public:
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
		float moveSpeed, float mouseSense, float pitch,
		float yaw, float fov
	);

	void ProcessMouse(float xOff, float yOff, bool constrain);
	void MoveCamera(CAM_MOVEMENT move, float dt);

	// Getters
	const glm::mat4 GetView();
	const float GetFOV();
	const float GetYaw();

	// For if the user would like to adjust the yaw manually!
	void SetYaw(float y);

private:
	glm::vec3 m_front, m_up;

	float m_moveSpeed = 2.5f;
	float m_pitch, m_yaw, m_mouseSense, m_fov;

	// Update the current lookat for the camera based on the pitch/yaw.
	void updateCameraFront();
};*/

#endif