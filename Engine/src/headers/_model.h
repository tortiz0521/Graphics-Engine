#ifndef _MODEL_H
#define _MODEL_H
#pragma once

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "_mesh.h"
#include "resource_manager.h"

class Model 
{
public:
    // Constructor for a model.
    Model(const char *path, ResourceManager &rm); // The entry point to our loading process!

    // Function to draw the model!
    //void Draw(Shader &s);

private:
    std::vector<Mesh> _meshes;
    std::string _directory;
    std::vector<std::shared_ptr<Texture>> _loadedTextures;
    ResourceManager *_rm;

    /*  
        IMPORTANT NOTE: Each node contains meshes that exist in the scene to allow for reusability!
        A mesh exists as a member of a scene, but each node controls its own meshes transform data.
        This creates a parent-child relationship with meshes. What we see here is a recursive process
        for extracting all of the data out of a model!
    */
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<std::shared_ptr<Texture>> processTextures(
        aiMaterial *mat, aiTextureType aiType, TextureType type);

};

#endif // _MODEL_H