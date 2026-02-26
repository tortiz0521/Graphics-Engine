#include "../headers/resource_manager.h"

// Process an entire model!
void ResourceManager::loadModel(std::string_view path, unsigned int persistentVBO)
{
    LoadedModel m = LoadedModel();

    std::string temp(path);
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(temp + temp.substr(temp.find_last_of('/'), temp.length() - 1) + ".obj",
        aiProcess_Triangulate);

    // Check to see that the scene was able to load the model properly.
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::'" << importer.GetErrorString() << "'\n";
        return;
    }

    m._name = temp;
    processNode(scene->mRootNode, scene, m, persistentVBO);
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(temp));
    m_models[ID] = std::make_unique<LoadedModel>(m);
}

void ResourceManager::processNode(
    aiNode *node, const aiScene *scene, LoadedModel &m, unsigned int persistentVBO) 
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m._meshes.push_back(processMesh(mesh, scene, m, persistentVBO));
    }

    for (unsigned int i = 0; i< node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, m, persistentVBO);
    }
}

Mesh ResourceManager::processMesh(
    aiMesh *mesh, const aiScene *scene, LoadedModel &m, unsigned int persistentVBO) 
{
    std::vector<Texture> texs{};
    std::vector<unsigned int> indices{};
    std::vector<Vertex> verts{};

    // Place all of the vertices into the vector of verts.
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        verts.push_back(Vertex(
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
                (mesh->mTextureCoords[0]) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f)
        ));
    }

    // Place all of the indices into the vector of indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    float shininess = 0.0f;
    // Setup up the models material!
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

        // Load the diffuse maps from the scenes material
        std::vector<Texture> maps = processTextures(
            mat, aiTextureType_DIFFUSE, DIFFUSE, m
        );
        texs.insert(texs.end(), maps.begin(), maps.end());
        maps.clear();

        // Load the normal maps from the scenes material
        maps = processTextures(mat, aiTextureType_NORMALS, NORMAL, m);
        texs.insert(texs.end(), maps.begin(), maps.end());
        maps.clear();

        // Load the specular maps from the scenes material
        maps = processTextures(mat, aiTextureType_SPECULAR, SPECULAR, m);
        texs.insert(texs.end(), maps.begin(), maps.end());
        maps.clear();

        if (AI_SUCCESS != mat->Get(AI_MATKEY_SHININESS, shininess)) {
            shininess = 32.0f;
        }
    }

    return Mesh(std::move(verts), std::move(indices), std::move(texs), persistentVBO, shininess);
}

std::vector<Texture> ResourceManager::processTextures(
    aiMaterial *mat, aiTextureType aiType, TextureType type, LoadedModel &m)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++) {
        aiString s;
        mat->GetTexture(aiType, i, &s);

        std::string path = m._name + "/" + std::string(s.C_Str());
        uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(path));
        auto tex = m_textures.find(ID);

        if (tex == m_textures.end()) {
            textures.push_back(LoadTexture(path, type));
        }
        else {
            textures.push_back(*tex->second.get());
        }
    }

    return textures;
}