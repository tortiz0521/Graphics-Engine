#include "../headers/camera.h"

#include <iostream>

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
    float moveSpeed, float mouseSense, float pitch, float yaw, float fov)
    : _pos(pos), _front(front), _up(up), _moveSpeed(moveSpeed), _mouseSense(mouseSense),
    _pitch(pitch), _yaw(yaw),  _fov(fov) {}

void Camera::ProcessMouse(float xOff, float yOff, bool constrain)
{
    this->_yaw += xOff;
    this->_pitch += yOff;

    // Constrain the pitch (y) value to not cause a flip in the LookAt!
    if (constrain) {
        if (this->_pitch > 89.0f)
           this->_pitch = 89.0f;
        else if (this->_pitch < -89.0f)
           this->_pitch = -89.0f;
    }

    this->updateCameraFront();
}

void Camera::MoveCamera(CAM_MOVEMENT m, float dt)
{
    //std::cout << _pos.x << ", " << _pos.y << ", " << _pos.z << '\n';
    float speed = dt * this->_moveSpeed;
    if (m == FORWARD)
        this->_pos.z -= speed;
    else if (m == BACKWARD)
        this->_pos.z += speed;
    else if (m == LEFT)
        this->_pos.x -= speed;
    else if (m == RIGHT)
        this->_pos.x += speed;
}

const glm::mat4 Camera::GetView()
{
    // The general calculation for a lookAt matrix! Instead of defining the lookAt axis ourselves,
    // glm can do this for us. Not necessarily best for FPS controls (Gimbal lock).
    return glm::lookAt(this->_pos, this->_pos + this->_front, this->_up);
}

const float Camera::GetFOV() { return this->_fov; }

const glm::vec3 Camera::GetPosition() { return this->_pos; }

const float Camera::GetYaw() { return this->_yaw; }

void Camera::SetYaw(float y) { this->_yaw = y; }

void Camera::updateCameraFront()
{
    glm::vec3 direction;
    // We need x/z to be shorter as y gets larger, so we multiply x/z by the cos(_pitch);
    direction.x = glm::cos(glm::radians(this->_yaw)) * glm::cos(glm::radians(this->_pitch));
    direction.y = glm::sin(glm::radians(this->_pitch));
    direction.z = glm::sin(glm::radians(this->_yaw)) * glm::cos(glm::radians(this->_pitch));

    this->_front = glm::normalize(direction);
    std::cout << _front.x << ", " << _front.y << ", " << _front.z << '\n';
}