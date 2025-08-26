#include "../headers/resource_manager.h"

// Process an entire model!
void ResourceManager::loadModel(std::string path)
{
    LoadedModel m = LoadedModel();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    // Check to see that the scene was able to load the model properly.
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::'" << importer.GetErrorString() << "'\n";
        return;
    }

    m._directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene, m);
    this->models[m._directory.c_str()] = std::make_unique<LoadedModel>(m);
}

void ResourceManager::processNode(
    aiNode *node, const aiScene *scene, LoadedModel &m) 
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m._meshes.push_back(processMesh(mesh, scene, m));
    }

    for (unsigned int i = 0; i< node->mNumMeshes; i++) {
        processNode(node->mChildren[i], scene, m);
    }
}

Mesh ResourceManager::processMesh(
    aiMesh *mesh, const aiScene *scene, LoadedModel &m) 
{
    std::vector<Texture> texs;
    std::vector<unsigned int> indices;
    std::vector<Vertex> verts;

    // Place all of the vertices into the vector of verts.
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        verts.push_back(Vertex(
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
            (mesh->mTextureCoords[0]) ? 
            glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f)
        ));
    }

    // Place all of the indices into the vector of indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; i++) {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    // Setup up the models material!
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

        // Load the diffuse maps from the scenes material
        std::vector<Texture> maps = processTextures(
            mat, aiTextureType_DIFFUSE, DIFFUSE, m
        );
        texs.insert(texs.end(), maps.begin(), maps.end());
        maps.clear();
        maps.shrink_to_fit(); // Might be overthinking this.

        // Load the normal maps from the scenes material
        maps = processTextures(mat, aiTextureType_NORMALS, NORMAL, m);
        texs.insert(texs.end(), maps.begin(), maps.end());
        maps.clear();
        maps.shrink_to_fit();

        // Load the specular maps from the scenes material
        maps = processTextures(mat, aiTextureType_SPECULAR, SPECULAR, m);
        texs.insert(texs.end(), maps.begin(), maps.end());
        maps.clear();
        maps.shrink_to_fit();
    }

    return Mesh (std::move(verts), std::move(indices), std::move(texs));
}

std::vector<Texture> ResourceManager::processTextures(
    aiMaterial *mat, aiTextureType aiType, TextureType type, LoadedModel &m)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++) {
        aiString s;
        mat->GetTexture(aiType, i, &s);

        std::string path = m._directory + "/" + std::string(s.C_Str());
        auto &tex = this->textures[path.c_str()];
        if (!tex) {
            textures.push_back(LoadTexture(m._directory, type));
        }
        else {
            textures.push_back(*tex.get());
        }
    }

    return textures;
}