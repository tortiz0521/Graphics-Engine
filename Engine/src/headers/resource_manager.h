#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#pragma once

#include "stb_image.h"
#include <unordered_map>
#include <string_view>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "_shader.h"
#include "_texture.h"
#include "_mesh.h"

struct TransparentHasher {
    using is_transparent = void; // Signals to std library containers that we are using a transparent comparator.
    size_t operator()(std::string_view sv) const {
        return std::hash<std::string_view>{}(sv);
    }
};

struct LoadedModel {
    std::vector<Mesh> _meshes;
    std::string_view _directory;
};

enum LightType {
    DIRECTION,
    POINT,
    SPOT
};

struct Light {
    LightType type;
    glm::vec3 Pos;
    glm::vec3 Dir;
};

class ResourceManager
{
public:
    ResourceManager() = default;

    // Load shaders/textures from their files
    const Shader& LoadShader(const char* vertex, const char* fragment, std::string_view name, const char* geometry = nullptr);
    const Texture& LoadTexture(std::string directory, TextureType type);

    // Get shaders/textures from their maps
    const Shader& GetShader(std::string_view name);
    const Texture& GetTexture(std::string_view path);

    // Load models into our 'cache'/Get models from out 'cache'
    const LoadedModel& LoadModel(std::string_view path);
    const LoadedModel& GetModel(std::string_view path);

private:
    /*  
        IMPORTANT NOTE: Each node contains meshes that exist in the scene to allow for reusability!
        A mesh exists as a member of a scene, but each node controls its own meshes transform data.
        This creates a parent-child relationship with meshes. What we see here is a recursive process
        for extracting all of the data out of a model!
    */
    void loadModel(std::string_view path);
    void processNode(aiNode *node, const aiScene *scene, LoadedModel& m);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene, LoadedModel& m);
    std::vector<Texture> processTextures(
        aiMaterial *mat, aiTextureType aiType, TextureType type, LoadedModel& m
    );

    // Maps for textures/shaders
    std::unordered_map<std::string, std::unique_ptr<Shader>, TransparentHasher, std::equal_to<>> shaders;
    std::unordered_map<std::string, std::unique_ptr<Texture>, TransparentHasher, std::equal_to<>> textures;
    std::unordered_map<std::string, std::unique_ptr<LoadedModel>, TransparentHasher, std::equal_to<>> models;
};

#endif