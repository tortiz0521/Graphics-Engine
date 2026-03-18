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
    Tracker<glm::vec3> pos{};
    glm::vec3 front{};
    glm::vec3 up{};
    float moveSpeed = 2.5f;
    float pitch{};
    float yaw{};
    float mouseSense{};
    float fov{};

    bool dirty = false;

    void read_pos(const std::vector<float>& val)
    {
        pos = { glm::vec3(val[0], val[1], val[2]), &dirty };
    };

    auto write_pos()
    {
        std::vector<float> val = { pos.value.x, pos.value.y, pos.value.z };
        return val;
    };

    void read_front(const std::vector<float>& val)
    {
        front = glm::vec3(val[0], val[1], val[2]);
    };

    auto write_front()
    {
        std::vector<float> val = { front.x, front.y, front.z };
        return val;
    };

    void read_up(const std::vector<float>& val)
    {
        up = glm::vec3(val[0], val[1], val[2]);
    };

    auto write_up()
    {
        std::vector<float> val = { up.x, up.y, up.z };
        return val;
    }

    CameraComponent() {};

    CameraComponent(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _up, float _pitch, float _yaw,
        float _mouseSense, float _FOV, float _moveSpeed = 2.5f)
    {
        pos = { _pos, &dirty };
        front = _front;
        up = _up;
        pitch = _pitch;
        yaw = _yaw;
        mouseSense = _mouseSense;
        fov = _FOV;
        moveSpeed = _moveSpeed;
    };

    CameraComponent(CameraComponent&& rhs) noexcept
    {
        pos = { rhs.pos.value, &dirty };
        front = rhs.front;
        up = rhs.up;
        pitch = rhs.pitch;
        yaw = rhs.yaw;
        mouseSense = rhs.mouseSense;
        fov = rhs.fov;
        moveSpeed = rhs.moveSpeed;
    };

    CameraComponent(const CameraComponent& rhs)
    {
        pos = { rhs.pos.value, &dirty };
        front = rhs.front;
        up = rhs.up;
        pitch = rhs.pitch;
        yaw = rhs.yaw;
        mouseSense = rhs.mouseSense;
        fov = rhs.fov;
        moveSpeed = rhs.moveSpeed;
    };

    CameraComponent& operator=(const CameraComponent& rhs)
    {
        pos = { rhs.pos.value, &dirty };
        front = rhs.front;
        up = rhs.up;
        pitch = rhs.pitch;
        yaw = rhs.yaw;
        mouseSense = rhs.mouseSense;
        fov = rhs.fov;
        moveSpeed = rhs.moveSpeed;

        return *this;
    };

    CameraComponent& operator=(CameraComponent&& rhs) noexcept
    {
        pos = { rhs.pos.value, &dirty };
        front = rhs.front;
        up = rhs.up;
        pitch = rhs.pitch;
        yaw = rhs.yaw;
        mouseSense = rhs.mouseSense;
        fov = rhs.fov;
        moveSpeed = rhs.moveSpeed;

        return *this;
    };
};

extern std::shared_ptr<ComponentManager<CameraComponent>> cameras;

#endif