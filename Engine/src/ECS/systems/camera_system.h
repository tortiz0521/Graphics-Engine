#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "../components/camera_component.h"
#include <functional>

namespace CameraSystem
{
	namespace {
		std::shared_ptr<CameraComponent> curCam = nullptr;

		void updateCameraFront(CameraComponent& cc)
		{
			glm::vec3 direction;
			// We need x/z to be shorter as y gets larger, so we multiply x/z by the cos(_pitch);
			direction.x = glm::cos(glm::radians(cc.yaw)) * glm::cos(glm::radians(cc.pitch));
			direction.y = glm::sin(glm::radians(cc.pitch));
			direction.z = glm::sin(glm::radians(cc.yaw)) * glm::cos(glm::radians(cc.pitch));

			cc.front = glm::normalize(direction);
		};
	};

	void ProcessMouse(float xOff, float yOff, bool constrain);

	void MoveCamera(CAM_MOVEMENT m, float dt);

	void UpdateCamView(std::function<void(const glm::mat4&)> UpdateCam);

	void UpdateCamProj(unsigned int WIDTH, unsigned int HEIGHT, std::function<void(const glm::mat4&)> UpdateProj);

	std::shared_ptr<CameraComponent>& SetCurrent(Entity e);
};

#endif