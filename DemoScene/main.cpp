#define STB_IMAGE_IMPLEMENTATION

#include "engine.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/emscripten.h>

typedef struct {
	std::vector<std::shared_ptr<TransformComponent>> planets;
	std::vector<std::shared_ptr<TransformComponent>> moons;
} userData;
#endif

void RunTestScene(std::shared_ptr<Scene> myScene);
void RunDemoScene_no_json(std::shared_ptr<Scene> myScene);
void RunDemoScene(std::shared_ptr<Scene> myScene);

void game_loop(std::vector<std::shared_ptr<TransformComponent>> planets, std::vector<std::shared_ptr<TransformComponent>> moons);

int main()
{
	std::shared_ptr<Scene> myScene = Engine::InitializeEngine(800, 600, { "demo_scene" }, "demo_scene", "../../assets/scenes/").scene;
	RunDemoScene_no_json(myScene);
	//Engine::SaveAllScenes();
	Engine::ShutDownEngine();

	return 0;
}

void RunTestScene(std::shared_ptr<Scene> myScene)
{
	std::vector<std::shared_ptr<LightComponent>> lights{};
	for (int i = 0; i < 3; i++) {
		Entity light = myScene->AddEntity();
		lights.emplace_back(myScene->AddComponent<LightComponent>(light,
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
			), Point));
	}

	std::vector<std::shared_ptr<TransformComponent>> models{};
	Entity prev = INVALID_ENTITY;
	for (int i = 2; i < 10; i++) {

		Entity e = myScene->AddEntity();
		models.emplace_back(myScene->AddComponent<TransformComponent>(e,
			TransformComponent(
				glm::vec3(float(i), float(i), -10.0f), // Translation
				glm::vec3(1.0f), // Scale
				0.0f // Rotation
			)));
		std::shared_ptr<RenderComponent> rc = myScene->AddComponent<RenderComponent>(e,
			RenderComponent{
				AssetRegistry::IDFromPath("../../assets/backpack"),
				AssetRegistry::IDFromPath("standard")
			}
		);

		myScene->AddComponent<HierarchyComponent>(e, HierarchyComponent{ prev, glm::mat4(1.0f) });
		prev = e;
	}

	Entity cam = myScene->AddEntity();
	std::shared_ptr<CameraComponent> cc = myScene->AddComponent<CameraComponent>(cam,
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

	Engine::SwitchScenes("test_scene");

	Engine::SetCurrentCamera(cam);

	//while (true) {
	//for (int i = 0; i < models.size(); i++) {
	//	models[i].get()->translation = glm::vec3(i * std::cos(glm::radians(float(glfwGetTime()) * 10.0f)), models[i].get()->translation.value.y,
	//		i * std::sin(glm::radians(float(glfwGetTime()) * 10.0f))
	//	);
	//}
	Engine::Render();
	//}
}


void RunDemoScene(std::shared_ptr<Scene> myScene)
{
	//Engine::SwitchScenes();

	// First, get organize the data.
	std::vector<std::shared_ptr<TransformComponent>> planets{}, moons{};
	Entity sun = hierarchy->GetEntity(0);

	for (int i = 1; i < hierarchy->GetCount(); i++) {
		if ((*hierarchy)[i]->parent == sun) {
			planets.emplace_back((*transforms)[i]);
		}
		else {
			moons.emplace_back((*transforms)[i]);
		}
	}

	Engine::SetCurrentCamera(cameras->GetEntity(0));

	// Then, run scene!
	game_loop(planets, moons);
}


void RunDemoScene_no_json(std::shared_ptr<Scene> myScene)
{
	Entity sun = myScene->AddEntity();
	std::shared_ptr<TransformComponent> sun_tc = myScene->AddComponent<TransformComponent>(sun,
		TransformComponent(
			glm::vec3(0.0f),
			glm::vec3(1.0f),
			0.0f
		)
	);

	myScene->AddComponent<RenderComponent>(sun,
		RenderComponent{
			AssetRegistry::IDFromPath("../../assets/backpack"),
			AssetRegistry::IDFromPath("standard_standard_")
		}
	);

	myScene->AddComponent<HierarchyComponent>(sun,
		HierarchyComponent{
			INVALID_ENTITY,
			glm::mat4(1.0f)
		}
	);

	std::vector<std::shared_ptr<TransformComponent>> planets{};
	for (int i = 0; i < 5; i++) {
		Entity p = myScene->AddEntity();
		planets.emplace_back(myScene->AddComponent<TransformComponent>(p,
			TransformComponent(
				glm::vec3(3.0f * float(i), 0.0f, 3.0f * float(i)),
				glm::vec3(0.1f),
				0.0f
			))
		);

		myScene->AddComponent<RenderComponent>(p,
			RenderComponent{
				AssetRegistry::IDFromPath("../../assets/planet"),
				AssetRegistry::IDFromPath("standard_standard_")
			}
		);

		myScene->AddComponent<HierarchyComponent>(p,
			HierarchyComponent{ sun, glm::mat4(1.0f) }
		);
	}

	std::vector<std::shared_ptr<TransformComponent>> moons{};
	for (int i = 0; i < planets.size(); i++) {
		for (int j = 0; j < 15; j++) {
			Entity m = myScene->AddEntity();
			moons.emplace_back(myScene->AddComponent<TransformComponent>(m,
				TransformComponent(
					glm::vec3(
						7.5f * std::cos(glm::radians((360.0f / 15.0f) * float(j))),
						-1.0f + ((2.0f / 15.0f) * j),
						7.5f * std::sin(glm::radians((360.0f / 15.0f) * float(j)))
					),
					glm::vec3(0.5f),
					0.0f
				))
			);

			myScene->AddComponent<RenderComponent>(m,
				RenderComponent{
					AssetRegistry::IDFromPath("../../assets/rock"),
					AssetRegistry::IDFromPath("standard_standard_")
				}
			);

			myScene->AddComponent<HierarchyComponent>(m,
				HierarchyComponent{ hierarchy->GetEntity(i + 1), glm::mat4(1.0f) }
			);
		}
	}

	Entity cam = myScene->AddEntity();
	std::shared_ptr<CameraComponent> cc = myScene->AddComponent<CameraComponent>(cam,
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

	Entity light = myScene->AddEntity();
	myScene->AddComponent<LightComponent>(light,
		LightComponent(Point, PointLight{
			glm::vec4(0.0f),
			glm::vec4(
			0.1f, // Constant
			0.05f, // Linear
			0.01f, 1.0f), // Quadratic
			glm::vec4(0.3f),
			glm::vec4(0.5f),
			glm::vec4(0.7f)
			}
		), Point);

	Engine::SetCurrentCamera(cam);

#ifdef __EMSCRIPTEN__
	auto loop = [](void*) {
		userData& data = *static_cast<userData*>(nullptr);
		game_loop(data.planets, data.moons);
	};
	userData data{ planets, moons };
	emscripten_set_main_loop_arg(loop, &data, 0, true);
#else
	game_loop(planets, moons);
#endif

	/*while (!Engine::WindowClosed()) {
		for (int i = 1; i < planets.size() + 1; i++) {
			planets[i - 1]->translation = glm::vec3(
				(2.0f * i) * std::cos(glm::radians(float(glfwGetTime()) * (i + 2.0f))),
				0.0f,
				(2.0f * i) * std::sin(glm::radians(float(glfwGetTime()) * (i + 2.0f)))
			);
		}

		for (int i = 0; i < moons.size(); i++) {
			int temp = i % 15;
			moons[i]->translation = glm::vec3(
				7.5f * std::cos((glm::radians((360.0f / 15.0f) * float(temp)) + glfwGetTime())),
				0.0f,
				7.5f * std::sin((glm::radians((360.0f / 15.0f) * float(temp)) + glfwGetTime()))
			);
		}

		Engine::Render();
	}*/
}

void game_loop(std::vector<std::shared_ptr<TransformComponent>> planets, std::vector<std::shared_ptr<TransformComponent>> moons)
{
	while (!Engine::WindowClosed()) {
		for (int i = 1; i < planets.size() + 1; i++) {
#ifdef __EMSCRIPTEN__
			planets[i - 1]->translation = glm::vec3(
				(2.0f * i) * std::cos(glm::radians(float(emscripten_get_now() * 0.001) * (i + 2.0f))),
				0.0f,
				(2.0f * i) * std::sin(glm::radians(float(emscripten_get_now() * 0.001) * (i + 2.0f)))
			);
#else
			planets[i - 1]->translation = glm::vec3(
				(2.0f * i) * std::cos(glm::radians(float(glfwGetTime()) * (i + 2.0f))),
				0.0f,
				(2.0f * i) * std::sin(glm::radians(float(glfwGetTime()) * (i + 2.0f)))
			);
#endif
		}

		for (int i = 0; i < moons.size(); i++) {
			int temp = i % 15;
#ifdef __EMSCRIPTEN__
			moons[i - 1]->translation = glm::vec3(
				(2.0f * i) * std::cos(glm::radians(float(emscripten_get_now() * 0.001) * (i + 2.0f))),
				0.0f,
				(2.0f * i) * std::sin(glm::radians(float(emscripten_get_now() * 0.001) * (i + 2.0f)))
			);
#else
			moons[i - 1]->translation = glm::vec3(
				(2.0f * i) * std::cos(glm::radians(float(glfwGetTime()) * (i + 2.0f))),
				0.0f,
				(2.0f * i) * std::sin(glm::radians(float(glfwGetTime()) * (i + 2.0f)))
			);
#endif
		}

		Engine::Render();
	}
}