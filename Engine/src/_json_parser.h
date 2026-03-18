#ifndef _JSON_PARSER_H
#define _JSON_PARSER_H

#include <glaze/glaze.hpp>
#include <optional>

#include "ECS/components/camera_component.h"
#include "ECS/components/hierarchy_component.h"
#include "ECS/components/light_component.h"
#include "ECS/components/render_component.h"
#include "ECS/components/transform_component.h"

template<>
struct glz::meta<CameraComponent>
{
	using T = CameraComponent;
	static constexpr auto value = glz::object(
		"pos", glz::custom<&T::read_pos, &T::write_pos>,
		"front", glz::custom<&T::read_front, &T::write_front>,
		"up", glz::custom<&T::read_up, &T::write_up>,
		"moveSpeed", &T::moveSpeed,
		"pitch", &T::pitch,
		"yaw", &T::yaw,
		"mouseSense", &T::mouseSense,
		"fov", &T::fov
	);
};

template<>
struct glz::meta<HierarchyComponent>
{
	using T = HierarchyComponent;
	static constexpr auto value = glz::object(
		"parent", &T::parent
	);
};

// Reflection metadata for LightComponent

template<>
struct glz::meta<DirectionLight>
{
	using T = DirectionLight;
	// Ambient
	static constexpr auto read_ambient = [](T& self, const std::vector<float>& val) -> void{ self.ambient = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_ambient = [](const T& self) -> auto {return std::vector<float>{ self.ambient.x, self.ambient.y, self.ambient.z }; };

	// Direction
	static constexpr auto read_direction = [](T& self, const std::vector<float>& val) -> void { self.direction = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_direction = [](const T& self) -> auto { return std::vector<float>{ self.direction.x, self.direction.y, self.direction.z }; };

	// Ambient
	static constexpr auto read_diffuse = [](T& self, const std::vector<float>& val) -> void { self.diffuse = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_diffuse = [](const T& self) -> auto {return std::vector<float>{ self.diffuse.x, self.diffuse.y, self.diffuse.z }; };

	// Direction
	static constexpr auto read_specular = [](T& self, const std::vector<float>& val) -> void { self.specular = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_specular = [](const T& self) -> auto { return std::vector<float>{ self.specular.x, self.specular.y, self.specular.z }; };

	static constexpr auto value = glz::object(
		"direction", glz::custom<read_direction, write_direction>,
		"ambient", glz::custom<read_ambient, write_ambient>,
		"diffuse", glz::custom<read_diffuse, write_diffuse>,
		"specular", glz::custom<read_specular, write_specular>
	);
};

template<>
struct glz::meta<PointLight>
{
	using T = PointLight;
	// Ambient
	static constexpr auto read_ambient = [](T& self, const std::vector<float>& val) -> void { self.ambient = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_ambient = [](const T& self) -> auto {return std::vector<float>{ self.ambient.x, self.ambient.y, self.ambient.z }; };

	// Position
	static constexpr auto read_pos = [](T& self, const std::vector<float>& val) -> void { self.pos = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_pos = [](const T& self) -> auto { return std::vector<float>{ self.pos.x, self.pos.y, self.pos.z }; };

	// Attenuation
	static constexpr auto read_attenuation = [](T& self, const std::vector<float>& val) -> void { self.attenuation = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_attenuation = [](const T& self) -> auto { return std::vector<float>{ self.attenuation.x, self.attenuation.y, self.attenuation.z }; };

	// Ambient
	static constexpr auto read_diffuse = [](T& self, const std::vector<float>& val) -> void { self.diffuse = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_diffuse = [](const T& self) -> auto {return std::vector<float>{ self.diffuse.x, self.diffuse.y, self.diffuse.z }; };

	// Direction
	static constexpr auto read_specular = [](T& self, const std::vector<float>& val) -> void { self.specular = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_specular = [](const T& self) -> auto { return std::vector<float>{ self.specular.x, self.specular.y, self.specular.z }; };

	static constexpr auto value = glz::object(
		"pos", glz::custom<read_pos, write_pos>,
		"attenuation", glz::custom<read_attenuation, write_attenuation>,
		"ambient", glz::custom<read_ambient, write_ambient>,
		"diffuse", glz::custom<read_diffuse, write_diffuse>,
		"specular", glz::custom<read_specular, write_specular>
	);
};

template<>
struct glz::meta<SpotLight>
{
	using T = SpotLight;
	// Ambient
	static constexpr auto read_ambient = [](T& self, const std::vector<float>& val) -> void { self.ambient = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_ambient = [](const T& self) -> auto {return std::vector<float>{ self.ambient.x, self.ambient.y, self.ambient.z }; };

	// Direction
	static constexpr auto read_direction = [](T& self, const std::vector<float>& val) -> void { self.pos = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_direction = [](const T& self) -> auto { return std::vector<float>{ self.pos.x, self.pos.y, self.pos.z }; };

	// Position
	static constexpr auto read_pos = [](T& self, const std::vector<float>& val) -> void { self.pos = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_pos = [](const T& self) -> auto { return std::vector<float>{ self.pos.x, self.pos.y, self.pos.z }; };

	// Attenuation
	static constexpr auto read_attenuation = [](T& self, const std::vector<float>& val) -> void { self.attenuation = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_attenuation = [](const T& self) -> auto { return std::vector<float>{ self.attenuation.x, self.attenuation.y, self.attenuation.z }; };

	// Ambient
	static constexpr auto read_diffuse = [](T& self, const std::vector<float>& val) -> void { self.diffuse = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_diffuse = [](const T& self) -> auto {return std::vector<float>{ self.diffuse.x, self.diffuse.y, self.diffuse.z }; };

	// Direction
	static constexpr auto read_specular = [](T& self, const std::vector<float>& val) -> void { self.specular = glm::vec4(val[0], val[1], val[2], 0.0f); };
	static constexpr auto write_specular = [](const T& self) -> auto { return std::vector<float>{ self.specular.x, self.specular.y, self.specular.z }; };

	static constexpr auto value = glz::object(
		"s_pos", glz::custom<read_pos, write_pos>,
		"s_direction", glz::custom<read_direction, write_direction>,
		"attenuation", glz::custom<read_attenuation, write_attenuation>,
		"ambient", glz::custom<read_ambient, write_ambient>,
		"diffuse", glz::custom<read_diffuse, write_diffuse>,
		"specular", glz::custom<read_specular, write_specular>,
		"inner_cut", &T::intCut,
		"outer_cut", &T::outCut
	);
};

template<>
struct glz::meta<LightComponent>
{
	using T = LightComponent;
	static constexpr auto value = glz::object(
		"type", &T::type,
		"data", glz::custom<&T::read_data, &T::write_data>
	);
};

template<>
struct glz::meta<RenderComponent>
{
	using T = RenderComponent;
	static constexpr auto value = glz::object(
		"model", glz::custom<&T::read_model, &T::write_model>,
		"shader", glz::custom<&T::read_shader, &T::write_shader>
	);
};

template<>
struct glz::meta<TransformComponent>
{
	using T = TransformComponent;
	static constexpr auto value = glz::object(
		"translation", glz::custom<&T::read_translation, &T::write_translation>,
		"scale", glz::custom<&T::read_scale, &T::write_scale>,
		"rotation", glz::custom<&T::read_rotation, &T::write_rotation>
	);
};

// SCHEMA - Entity configuration

struct EntityConfig
{
	std::optional<HierarchyComponent> hierarchy;
	std::optional<LightComponent> light;
	std::optional<RenderComponent> render;
	std::optional<TransformComponent> transform;
	std::optional<CameraComponent> camera;

};

struct SceneEntities
{
	std::vector<EntityConfig> entities;
};


// SCHEMA - Manifest config

struct ShaderManifestConfig
{

	std::string_view vertex_path;
	std::string_view fragment_path;
	std::optional<std::string_view> geo_path;
};


struct TextureManifestConfig
{
	std::string_view tex_path;
	TextureType type;
};

struct ManifestConfig
{
	std::vector<ShaderManifestConfig> shader_manifest;
	std::vector<std::string_view> model_manifest;
	std::vector<TextureManifestConfig> texture_manifest;
};

#endif