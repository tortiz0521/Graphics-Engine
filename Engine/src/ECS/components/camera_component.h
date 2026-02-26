#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "../_component_manager.h"

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

enum CAM_MOVEMENT {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct CameraComponent
{
    Tracker<glm::vec3> pos;
    glm::vec3 front;
    glm::vec3 up;
    float moveSpeed = 2.5f;
    float pitch;
    float yaw;
    float mouseSense;
    float fov;

    bool dirty = false;

    CameraComponent() {};
    CameraComponent(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _up, float _pitch, float _yaw,
        float _mouseSense, float _FOV, float _moveSpeed = 2.5f) 
    : front(_front), up(_up), pitch(_pitch), yaw(_yaw), mouseSense(_mouseSense), fov(_FOV), moveSpeed(_moveSpeed)
    {
        pos = { _pos, &dirty };
    }
};

extern ComponentManager<CameraComponent> cameras;

#endif