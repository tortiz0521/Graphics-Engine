#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#pragma once

#include "stb_image.h"
#include <unordered_map>
#include <string_view>
#include <memory>
#include <algorithm>
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
    uint32_t operator()(std::string_view sv) const {
        return static_cast<uint32_t>(std::hash<std::string_view>{}(sv));
    }
};

static const uint32_t INVALID_ID = -1;

struct AssetRegistry
{
    static std::unordered_map<std::string, uint32_t> path_ID;
    static std::unordered_map<uint32_t, std::string> ID_path;

    static uint32_t RegisterPath(std::string_view path)
    {
        auto it = path_ID.find(std::string(path));
        if (it != path_ID.end())
            return it->second;

        uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(path));
        path_ID.emplace(path, ID);
        ID_path.emplace(ID, path);

        return ID;
    };

    static std::string_view PathFromID(uint32_t id)
    {
        return ID_path[id];
    };

    static uint32_t IDFromPath(std::string_view s)
    {
        return path_ID[std::string(s)];
    }
};

struct LoadedModel {
    std::vector<Mesh> _meshes;
    std::string _name;

    void DeleteModel() {
        for (Mesh m : _meshes) {
            m.DeleteMesh();
        }
    }
};

class ResourceManager
{
public:
    ResourceManager() = default;

    // Load shaders/textures from their files
    const uint32_t LoadShader(const char* vertex, const char* fragment, std::string_view name, const char* geometry = nullptr);
    std::shared_ptr<Texture> LoadTexture(std::string directory, TextureType type);
    const uint32_t LoadTexture_ID(std::string directory, TextureType type);
    const uint32_t LoadModel(std::string_view path, unsigned int persistentVBO);

    // Get shaders/textures from their maps
    std::shared_ptr<Shader> GetShader(uint32_t ID);
    std::shared_ptr<Texture> GetTexture(uint32_t ID);
    std::shared_ptr<LoadedModel> GetModel(uint32_t ID);

    std::shared_ptr<Shader> GetShader(std::string_view path);
    std::shared_ptr<Texture> GetTexture(std::string_view path);
    std::shared_ptr<LoadedModel> GetModel(std::string_view path);


    void ReleaseResources();
private:
    /*  
        IMPORTANT NOTE: Each node contains meshes that exist in the scene to allow for reusability!
        A mesh exists as a member of a scene, but each node controls its own meshes transform data.
        This creates a parent-child relationship with meshes. What we see here is a recursive process
        for extracting all of the data out of a model!
    */
    void loadModel(std::string_view path, unsigned int persistentVBO);
    void processNode(aiNode *node, const aiScene *scene, LoadedModel& m, unsigned int persistentVBO);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene, LoadedModel& m, unsigned int persistentVBO);
    std::vector<std::shared_ptr<Texture>> processTextures(
        aiMaterial *mat, aiTextureType aiType, TextureType type, LoadedModel& m
    );

    // ID Based maps for general resources.
    std::unordered_map<uint32_t, std::shared_ptr<Shader>> m_shaders;
    std::unordered_map<uint32_t, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<uint32_t, std::shared_ptr<LoadedModel>> m_models;
};


class SceneResourceManager
{
public:
    SceneResourceManager() = default;

    std::vector<uint32_t> m_shaderIDs{}; // Keep track of shaders and their ordering.
    std::vector<std::shared_ptr<Shader>> m_sceneShaders{}; // Reference the components themselves.

    std::vector<uint32_t> m_texIDs{};
    std::vector<std::shared_ptr<Texture>> m_sceneTexs{};

    std::vector<uint32_t> m_modelIDs{};
    std::vector<std::shared_ptr<LoadedModel>> m_sceneModels{};

    void SetModelIDs(std::vector<uint32_t> ids, ResourceManager& rm);
    void SetShaderIDs(std::vector<uint32_t> ids, ResourceManager& rm);

    void UpdateSceneModels(std::vector<uint32_t> models, ResourceManager& rm);
    void UpdateSceneShaders(std::vector<uint32_t> shaders, ResourceManager& rm);

    // Load and Unload Scene resource manager
    void SceneLoad(ResourceManager& rm);
    void SceneUnload();
};

#endif