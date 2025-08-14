#ifndef RENDERER_H
#define REDERER_H
#pragma once

#include <memory>
#include <glad/glad.h>

#include "_model.h"

class Renderer
{
public:
    Renderer() = default;
    void Draw(Model &m, const Shader &s, const glm::vec3 &position,
        const glm::vec3 &size,const glm::vec3 color);

private:
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