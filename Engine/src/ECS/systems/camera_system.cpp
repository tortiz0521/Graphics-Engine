#include "camera_system.h"
#include <iostream>

void CameraSystem::ProcessMouse(float xOff, float yOff, bool constrain)
{;
	if (!curCam)
		return;

	curCam->yaw += xOff;
	curCam->pitch += yOff;

	// Constrain the pitch (y) value to not cause a flip in the LookAt!
	if (constrain) {
		if (curCam->pitch > 89.0f)
			curCam->pitch = 89.0f;
		else if (curCam->pitch < -89.0f)
			curCam->pitch = -89.0f;
	}
	updateCameraFront(*curCam);
}

void CameraSystem::MoveCamera(CAM_MOVEMENT m, float dt)
{
	if (!curCam)
		return;

	glm::vec3 xzLook = glm::normalize(
		glm::vec3(cos(glm::radians(curCam->yaw)), 0.0f, sin(glm::radians(curCam->yaw)))
	);

	float speed = dt * curCam->moveSpeed;
	if (m == FORWARD)
		curCam->pos.value += speed * xzLook;
	else if (m == BACKWARD)
		curCam->pos.value -= speed * xzLook;
	else if (m == LEFT)
		curCam->pos.value -= speed * glm::normalize(glm::cross(xzLook, curCam->up));
	else if (m == RIGHT)
		curCam->pos.value += speed * glm::normalize(glm::cross(xzLook, curCam->up));
}

void CameraSystem::UpdateCamView(std::function<void(const glm::mat4&)> UpdateCam)
{
	if (!curCam) {
		std::cout << "ERROR::UNABLE_TO_UPDATE_CAM_UBO::NO_CURRENT_CAMERA" << std::endl;
		return;
	}

	UpdateCam(glm::lookAt(curCam->pos.value, curCam->pos.value + curCam->front, curCam->up));
}

void CameraSystem::UpdateCamProj(unsigned int WIDTH, unsigned int HEIGHT, std::function<void(const glm::mat4&)> UpdateProj)
{
	if (!curCam) {
		std::cout << "ERROR::UNABLE_TO_UPDATE_CAM_UBO::NO_CURRENT_CAMERA" << std::endl;
		return;
	}

	glm::mat4 projection = glm::perspective(glm::radians(curCam->fov),
		(float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	UpdateProj(projection);
}

std::shared_ptr<CameraComponent>& CameraSystem::SetCurrent(Entity e)
{
	curCam = cameras.get()->GetComponent(e);
	return curCam;
}