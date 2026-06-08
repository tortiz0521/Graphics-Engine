#include "../resource/resource_manager.h"

std::unordered_map<std::string, uint32_t> AssetRegistry::path_ID;
std::unordered_map<uint32_t, std::string> AssetRegistry::ID_path;

void ResourceManager::ReleaseResources()
{
    for (std::unordered_map<uint32_t, std::shared_ptr<Shader>>::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it) {
        it->second->DeleteShaderProgram();
    }

    for (std::unordered_map<uint32_t, std::shared_ptr<Texture>>::iterator it = m_textures.begin(); it != m_textures.end(); ++it) {
        it->second->DeleteTexture();
    }

    for (std::unordered_map<uint32_t, std::shared_ptr<LoadedModel>>::iterator it = m_models.begin(); it != m_models.end(); ++it) {
        it->second->DeleteModel();
    }
}

const uint32_t ResourceManager::LoadShader(const char *vertex, const char *fragment, std::string_view name, const char * geometry)
{
    uint32_t ID = AssetRegistry::RegisterPath(name);
    if (m_shaders[ID]) {
        return ID;
    }

    std::string vString, fString, gString;
    std::ifstream vfile, ffile, gfile;

    vfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ffile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if(geometry != nullptr)
        gfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        std::stringstream vStream, fStream, gStream;

        vfile.open(vertex);
        vStream << vfile.rdbuf();
        vfile.close();
        vString = vStream.str();

        ffile.open(fragment);
        fStream << ffile.rdbuf();
        ffile.close();
        fString = fStream.str();

        if (geometry != nullptr) {
            gfile.open(geometry);
            gStream << gfile.rdbuf();
            gfile.close();
            gString = gStream.str();
        }
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_READ_SUCCESFULLY::'" << e.what() << "'\n";
    }

    const char* vCode = vString.c_str(), * fCode = fString.c_str(), * gCode{};
    if (geometry != nullptr)
        gCode = gString.c_str();

    std::shared_ptr<Shader> s = std::make_shared<Shader>(Shader());
    s->Compile(vCode, fCode, gCode == nullptr ? nullptr : gCode);

    // Hash ID from file name + place new shader in container.
    m_shaders[ID] = s;
    return ID;

}

std::shared_ptr<Texture> ResourceManager::LoadTexture(std::string p, TextureType type)
{
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(p));
    if (m_textures[ID]) {
        return nullptr;
    }

    std::shared_ptr<Texture> t = std::make_shared<Texture>(Texture());

    int width, height, nrComp;
        
    unsigned char *data = stbi_load(p.c_str(), &width, &height, &nrComp, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrComp == 1)
            format = GL_RED;
        else if (nrComp == 3)
            format = GL_RGB;
        else if (nrComp == 4)
            format = GL_RGBA;

        t->Generate(width, height, format, data, type, p); // Generate Texture
    }
    else {
        std::cout << "Here is the error: " << stbi_failure_reason() << '\n';
        std::cout << "Texture failed to load at path: " << p << '\n';
    }

    stbi_image_free(data);

    // Hash ID fron file name + place new Texture in container.
    m_textures[ID] = t;
    return m_textures[ID];
}

const uint32_t ResourceManager::LoadTexture_ID(std::string directory, TextureType type)
{
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(directory));
    if (m_textures[ID]) {
        return ID;
    }

    std::shared_ptr<Texture> t = std::make_shared<Texture>(Texture());

    int width, height, nrComp;

    unsigned char* data = stbi_load(directory.c_str(), &width, &height, &nrComp, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrComp == 1)
            format = GL_RED;
        else if (nrComp == 3)
            format = GL_RGB;
        else if (nrComp == 4)
            format = GL_RGBA;

        t->Generate(width, height, format, data, type, directory); // Generate Texture
    }
    else {
        std::cout << "Here is the error: " << stbi_failure_reason() << '\n';
        std::cout << "Texture failed to load at path: " << directory << '\n';
    }

    stbi_image_free(data);

    // Hash ID fron file name + place new Texture in container.
    m_textures[ID] = t;
    return ID;
}

const uint32_t ResourceManager::LoadModel(std::string_view path, unsigned int persistentVBO)
{
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(path));
    if(m_models.find(ID) == m_models.end())
         loadModel(path, persistentVBO);
    return ID;
}

// Loaded Model
std::shared_ptr<LoadedModel> ResourceManager::GetModel(uint32_t ID)
{
    return m_models[ID];
}

std::shared_ptr<LoadedModel> ResourceManager::GetModel(std::string_view path)
{
    return m_models[static_cast<uint32_t>(std::hash<std::string_view>{}(path))];
}

// Shader
std::shared_ptr<Shader> ResourceManager::GetShader(uint32_t ID)
{
    return m_shaders[ID];
}

std::shared_ptr<Shader> ResourceManager::GetShader(std::string_view path)
{
    return m_shaders[static_cast<uint32_t>(std::hash<std::string_view>{}(path))];
}

// Texture
std::shared_ptr<Texture> ResourceManager::GetTexture(uint32_t ID)
{
    return m_textures[ID];
}

std::shared_ptr<Texture> ResourceManager::GetTexture(std::string_view path)
{
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(path));
    return m_textures[ID];
}


// SCENE_RESOURCE_MANAGER

void SceneResourceManager::SetModelIDs(std::vector<uint32_t> ids, ResourceManager& rm)
{
    for (uint32_t id : ids) {
        m_modelIDs.emplace_back(id);
        if (m_modelIDs.size() == 1)
            continue;

        for (int j = m_modelIDs.size() - 1; j > 0; j--) {
            if (j - 1 < 0)
                break;

            if (m_modelIDs[j - 1] > m_modelIDs[j])
                std::swap(m_modelIDs[j], m_modelIDs[j - 1]);
            else break;
        }
    }
}

void SceneResourceManager::SetShaderIDs(std::vector<uint32_t> ids, ResourceManager& rm)
{
    for (uint32_t id : ids) {
        m_shaderIDs.emplace_back(id);
        if (m_shaderIDs.size() == 1)
            continue;

        for (int j = m_shaderIDs.size() - 1; j > 0; j--) {
            if (j - 1 < 0)
                break;

            if (m_shaderIDs[j - 1] < m_shaderIDs[j])
                std::swap(m_shaderIDs[j], m_shaderIDs[j - 1]);
            else break;
        }
    }
}


void SceneResourceManager::UpdateSceneModels(std::vector<uint32_t> models, ResourceManager& rm)
{
    for (uint32_t id : models) {
        if (m_modelIDs.size() == 0) {
            m_modelIDs.emplace_back(id);
            m_sceneModels.emplace_back(rm.GetModel(id));
            continue;
        }

        m_modelIDs.emplace_back(id);
        m_sceneModels.emplace_back(rm.GetModel(id));
        // Reverse bubble sort
        for (int j = m_modelIDs.size() - 1; j > 0; j--) {
            if (j - 1 < 0)
                break;

            if (m_modelIDs[j - 1] < m_modelIDs[j]) {
                std::swap(m_modelIDs[j], m_modelIDs[j - 1]);
                std::swap(m_sceneModels[j], m_sceneModels[j - 1]);
            }
            else break;
        }
    }
}

void SceneResourceManager::UpdateSceneShaders(std::vector<uint32_t> shaders, ResourceManager& rm)
{
    for (uint32_t id : shaders) {
        if (m_modelIDs.size() == 0) {
            m_shaderIDs.emplace_back(id);
            m_sceneShaders.emplace_back(rm.GetShader(id));
        }

        m_shaderIDs.emplace_back(id);
        m_sceneShaders.emplace_back(rm.GetShader(id));
        // Reverse bubble sort
        for (int j = m_shaderIDs.size() - 1; j > 0; j--) {
            if (j - 1 < 0)
                break;

            if (m_shaderIDs[j - 1] < m_shaderIDs[j]) {
                std::swap(m_shaderIDs[j], m_shaderIDs[j - 1]);
                std::swap(m_sceneShaders[j], m_sceneShaders[j - 1]);
            }
            else break;
        }
    }
}

void SceneResourceManager::SceneUnload()
{
    m_sceneModels = {};
    m_sceneShaders = {};
    m_sceneTexs = {};
}

void SceneResourceManager::SceneLoad(ResourceManager& rm)
{
    for (uint32_t id : m_modelIDs)
        m_sceneModels.emplace_back(rm.GetModel(id));

    for (uint32_t id : m_shaderIDs)
        m_sceneShaders.emplace_back(rm.GetShader(id));

    for (uint32_t id : m_texIDs)
        m_sceneTexs.emplace_back(rm.GetTexture(id));
}