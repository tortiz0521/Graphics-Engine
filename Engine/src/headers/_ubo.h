#ifndef _UBO_H
#define _UBO_H
#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <variant>
#include <iostream>

// This is used to ensure that there is no allignment optimizing being done so it matches GPU memory.

struct DirectionLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec4 pos;
    glm::vec4 attenuation;
    //float constant;
    //float linear;
    //float quadratic;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

struct SpotLight {
    glm::vec3 pos;
    glm::vec3 direction;
    float intCut;
    float outCut;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

typedef std::variant<DirectionLight, PointLight, SpotLight> LightVariant;

struct CameraData {
    glm::mat4 proj;
    glm::mat4 view;
};

template<typename T>
class UniformBuffer {
public:
    UniformBuffer(size_t bufferSize, uint32_t bindNum);
    void BindBuffer();

    // Take a FULL STRUCT to update a structs UBO data!
    UniformBuffer& UpdateStruct(const T& data, size_t index);
    // Take some specific piece of data within a struct and update that data.
    UniformBuffer& UpdateField(size_t offset, size_t size, const void* data, size_t index);
    // Add a new light to the UBO.
    bool AddStruct(const T& data);
    // Primarily used for if a light has been removed! Good for ensuring that data will be packed back together with space to allow for AddStruct() to work.
    UniformBuffer& ResetBuffer(const std::vector<T>& dataList);

private:
    unsigned int m_UBO;
    size_t m_bufferMax, m_curSize;
    uint32_t m_bind;
};

// Bind the buffer once to a binding point! As long as each shader uses the same bindings, there is no need to change this.
template<typename T>
UniformBuffer<T>::UniformBuffer(size_t bufferSize, uint32_t bindNum) : m_bufferMax(bufferSize), m_bind(bindNum), m_curSize(0)
{
    glGenBuffers(1, &m_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize * sizeof(T), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_bind, m_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename T>
void UniformBuffer<T>::BindBuffer()
{
    glBindBufferBase(GL_UNIFORM_BUFFER, m_bind, m_UBO);
}

template<typename T>
UniformBuffer<T>& UniformBuffer<T>::UpdateStruct(const T& data, size_t index)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(T) * index, sizeof(T), &data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return *this;
}

template<typename T>
UniformBuffer<T>& UniformBuffer<T>::UpdateField(size_t offset, size_t size, const void* data, size_t index)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, (sizeof(T) * index) + offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return *this;
}

template<typename T>
bool UniformBuffer<T>::AddStruct(const T& data)
{
    if (m_curSize == m_bufferMax)
        return false;

    UpdateStruct(data, m_curSize);
    m_curSize++;
    return true;
}

template<typename T>
UniformBuffer<T>& UniformBuffer<T>::ResetBuffer(const std::vector<T>& dataList)
{
    // First, reset buffer
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferData(GL_UNIFORM_BUFFER, m_bufferMax * sizeof(T), NULL, GL_STATIC_DRAW);

    // Next, add the current data in our list back into the buffer.
    m_curSize = dataList.size();
    for (uint32_t i = 0; i < m_curSize; i++) {
        UpdateStruct(dataList[i], i);
    }

    return *this;
}

#endif