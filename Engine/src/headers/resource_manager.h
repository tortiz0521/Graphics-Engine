#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#pragma once

#include "stb_image.h"
#include <unordered_map>
#include <string_view>
#include <memory>
#include <any>
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
#include "_ubo.h"

struct TransparentHasher {
    using is_transparent = void; // Signals to std library containers that we are using a transparent comparator.
    size_t operator()(std::string_view sv) const {
        return std::hash<std::string_view>{}(sv);
    }
};

struct IDHasher {
    using is_transparent = void;
    uint32_t operator()(const std::string_view& sv) const {
        size_t h = std::hash<std::string_view>{}(sv);
        return static_cast<uint32_t>(h);
    }
};

struct IDEqual {
    bool operator()(const uint32_t a, const uint32_t b) {
        return a == b;
    }
};

struct LoadedModel {
    std::vector<Mesh> _meshes;
    std::string _name;
};

class ResourceManager
{
public:
    ResourceManager() = default;

    // Load shaders/textures from their files
    const uint32_t LoadShader(const char* vertex, const char* fragment, std::string_view name, const char* geometry = nullptr);
    const Texture& LoadTexture(std::string directory, TextureType type);
    const uint32_t LoadModel(std::string_view path, unsigned int persistentVBO);

    // Get shaders/textures from their maps
    const Shader& GetShader(uint32_t ID);
    const Texture& GetTexture(std::string_view path);
    const LoadedModel& GetModel(uint32_t ID);

private:
    // Logic for creating a Uniform Buffer Object...
    template<typename T> const UniformBuffer<T>& loadUniformBuffer(std::string_view path);
    template<typename T> const UniformBuffer<T>& getUniformBuffer(uint32_t ID);

    /*  
        IMPORTANT NOTE: Each node contains meshes that exist in the scene to allow for reusability!
        A mesh exists as a member of a scene, but each node controls its own meshes transform data.
        This creates a parent-child relationship with meshes. What we see here is a recursive process
        for extracting all of the data out of a model!
    */
    void loadModel(std::string_view path, unsigned int persistentVBO);
    void processNode(aiNode *node, const aiScene *scene, LoadedModel& m, unsigned int persistentVBO);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene, LoadedModel& m, unsigned int persistentVBO);
    std::vector<Texture> processTextures(
        aiMaterial *mat, aiTextureType aiType, TextureType type, LoadedModel& m
    );

    // Maps for textures/shaders
    std::unordered_map<std::string, std::unique_ptr<Shader>, TransparentHasher, std::equal_to<>> shaders;
    std::unordered_map<std::string, std::unique_ptr<Texture>, TransparentHasher, std::equal_to<>> textures;
    std::unordered_map<std::string, std::unique_ptr<LoadedModel>, TransparentHasher, std::equal_to<>> models;

    // ID Based maps for general resources.
    std::unordered_map<uint32_t, std::unique_ptr<Shader>> m_shaders;
    std::unordered_map<uint32_t, std::unique_ptr<Texture>> m_textures;
    std::unordered_map<uint32_t, std::unique_ptr<LoadedModel>> m_models;
    std::unordered_map<uint32_t, std::unique_ptr<std::any>> m_ubos;
};

#endif