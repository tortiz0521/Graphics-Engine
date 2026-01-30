#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include <memory>
#include <glad/glad.h>

#include "resource_manager.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


class Renderer
{
public:
    Renderer() = default;

    virtual void Draw(const LoadedModel &m, const Shader &s, const glm::vec3 &position,
        const glm::vec3 &size, const glm::vec3 color);

    void DrawInstanced(uint32_t modelID, uint32_t shaderID, const std::vector<glm::mat4>& data);

    void InitRenderer();

private:
    std::unique_ptr<GLFWwindow> m_window = nullptr;
    unsigned int m_DynamicVBO;
    ResourceManager m_manager;

    void UpdateVBO(const std::vector<glm::mat4>& data);
};

class BoxRenderer : Renderer
{
public:
    BoxRenderer();
    void Draw(const Shader &s, const glm::vec3 &position,
        const glm::vec3 &size,const glm::vec3 color, GLFWwindow* curr);

    void initRenderer();
        
private:
    unsigned int _VAO;
};

#endif // RENDERER_H