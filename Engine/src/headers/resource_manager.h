#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#pragma once

#include "stb_image.h"
#include <unordered_map>
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

struct LoadedModel {
    std::vector<Mesh> _meshes;
    std::string _directory;
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
    const Shader& LoadShader(const char* vertex, const char* fragment, const char* name, const char* geometry = nullptr);
    const Texture& LoadTexture(std::string directory, TextureType type);

    // Get shaders/textures from their maps
    const Shader& GetShader(const char *name);
    const Texture& GetTexture(const char *path);

    // Load models into our 'cache'/Get models from out 'cache'
    const LoadedModel& LoadModel(const char *path);
    const LoadedModel& GetModel(const char *path);

private:
    /*  
        IMPORTANT NOTE: Each node contains meshes that exist in the scene to allow for reusability!
        A mesh exists as a member of a scene, but each node controls its own meshes transform data.
        This creates a parent-child relationship with meshes. What we see here is a recursive process
        for extracting all of the data out of a model!
    */
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene, LoadedModel& m);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene, LoadedModel& m);
    std::vector<Texture> processTextures(
        aiMaterial *mat, aiTextureType aiType, TextureType type, LoadedModel& m
    );

    // Maps for textures/shaders
    std::unordered_map<const char*, std::unique_ptr<Shader>> shaders;
    std::unordered_map<const char*, std::unique_ptr<Texture>> textures;
    std::unordered_map<const char*, std::unique_ptr<LoadedModel>> models;
};

#endif