#ifndef CAMERA_H
#define CAMERA_H
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CAM_MOVEMENT {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
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
    const glm::vec3 GetPosition();
    const float GetYaw();

    // For if the user would like to adjust the yaw manually!
    void SetYaw(float y);

private:
    glm::vec3 _pos, _front, _up;
    float _moveSpeed = 2.5f;
    float _pitch, _yaw, _mouseSense, _fov;

    // Update the current lookat for the camera based on the pitch/yaw.
    void updateCameraFront();
};

#endif