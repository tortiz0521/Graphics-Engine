#ifndef _MESH_H
#define _MESH_H
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "_texture.h"
#include "_shader.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

    Vertex(glm::vec3 v, glm::vec3 n, glm::vec2 tex)
        : position(v), normal(n), texCoord(tex) {};
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> && vertices, std::vector<unsigned int> && inidces,
        std::vector<std::shared_ptr<Texture>> && textures);

    void Draw(const Shader &s); // Add the specific renderer that we are going to use to the param list!

private:
    unsigned int _VAO, _VBO, _EBO;
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::vector<std::shared_ptr<Texture>> _textures;

    void setupMesh();
};

#endif // _MESH_H