#define STB_IMAGE_IMPLEMENTATION

#include "../headers/renderer.h"
#include "../headers/resource_manager.h"
#include "../headers/scene.h"


int main()
{
	Scene myScene{};
	Renderer r{};
	r.InitRenderer(800, 600);

	const Texture& con2 = myScene.m_manager.get()->LoadTexture("../../assets/textures/container2.png", NONE);

	const Texture& con2_spec = myScene.m_manager.get()->LoadTexture("../../assets/textures/container2_specular.png", NONE);

	// Update the resource manager:
	const uint32_t box = myScene.m_manager.get()->LoadShader("../../assets/shaders/box.vs",
		"../../assets/shaders/box.fs", "box");

	const uint32_t standard = myScene.m_manager.get()->LoadShader("../../assets/shaders/standard.vs",
		"../../assets/shaders/standard.fs", "standard");

	const uint32_t backpack = myScene.m_manager.get()->LoadModel("../../assets/backpack", r.GetDynamicVBO());

	for (int i = 0; i < 3; i++) {
		Entity light = myScene.AddEntity();
		std::shared_ptr<LightComponent> lc = myScene.AddComponent<LightComponent>(light, r,
			LightComponent(Point, PointLight{
				glm::vec4(10.0f * std::cos(glm::radians(120.0f * float(i))), 1.0f, 10.0f * std::sin(glm::radians(120.0f * float(i))
				), 1.0f),
				glm::vec4(
				0.1f, // Constant
				0.05f, // Linear
				0.01f, 1.0f), // Quadratic
				glm::vec4(0.3f),
				glm::vec4(0.5f),
				glm::vec4(0.7f)
			}
		), Point);
	}

	std::vector<std::shared_ptr<TransformComponent>> models{};
	for (int i = 0; i < 10; i++) {
		if (i == 0)
			continue;

		Entity e = myScene.AddEntity();
		myScene.AddComponent<HierarchyComponent>(e, r);
		models.emplace_back(myScene.AddComponent<TransformComponent>(e, r,
			TransformComponent(
				glm::vec3(float(i), float(i), -10.0f), // Translation
				glm::vec3(1.0f), // Scale
				0.0f // Rotation
		)));
		std::shared_ptr<RenderComponent> rc = myScene.AddComponent<RenderComponent>(e, r, RenderComponent{backpack, standard});
	}

	Entity cam = myScene.AddEntity();
	std::shared_ptr<CameraComponent> cc = myScene.AddComponent<CameraComponent>(cam, r,
		CameraComponent(
			glm::vec3(0.0f), // Pos
			glm::vec3(0.0f, 0.0f, -1.0f), // Front
			glm::vec3(0.0f, 1.0f, 0.0f), // Up
			0.0f, // Pitch
			-90.0f, // Yaw (set to -90.0f to ensure the Y-component is not broken)
			0.1f, // Mouse sense
			45.0f, // FOV
			2.5f // Move speed
		));

	myScene.SetCamera(cam);
	myScene.SetCamUBO(800, 600, r);

	while (true) {
		for (int i = 0; i < models.size(); i++) {

			models[i].get()->translation = glm::vec3(i * std::cos(glm::radians(float(glfwGetTime()) * 10.0f)), models[i].get()->translation.value.y,
				i * std::sin(glm::radians(float(glfwGetTime()) * 10.0f))
			);
		}
		myScene.RenderScene(r);
	}

	return 0;
}