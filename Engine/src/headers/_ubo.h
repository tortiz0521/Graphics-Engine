#ifndef _UBO_H
#define _UBO_H
#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

// This is used to ensure that there is no allignment optimizing being done so it matches GPU memory.
#pragma pack(push, 0)

struct DirectionLight {
    glm::vec4 direction;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

struct PointLight {
    glm::vec4 pos;
    float constant;
    float linear;
    float quadratic;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

struct SpotLight {
    glm::vec4 pos;
    glm::vec4 direction;
    float intCut;
    float outCut;
    float constant;
    float linear;
    float quadratic;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

struct CameraData {
    glm::mat4 proj;
    glm::mat4 view;
};

#pragma pack(pop)

template<typename T>
class UniformBuffer {
public:
    UniformBuffer();

    // Take a FULL STRUCT to update the ENTIRE UBO data!
    UniformBuffer& UpdateStruct(T& data);
    // Take some specific piece of data within the struct and update that data.
    UniformBuffer& UpdateField(size_t offset, size_t size, const void* data);

private:
    unsigned int m_UBO;
};


template<typename T>
UniformBuffer<T>::UniformBuffer()
{
    glGenBuffers(1, &m_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename T>
UniformBuffer<T>& UniformBuffer<T>::UpdateStruct(T& data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename T>
UniformBuffer<T>& UniformBuffer<T>::UpdateField(size_t offset, size_t size, const void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

#endif