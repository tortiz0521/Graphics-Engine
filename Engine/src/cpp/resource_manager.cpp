#include "../headers/resource_manager.h"

const uint32_t ResourceManager::LoadShader(const char *vertex, const char *fragment, std::string_view name, const char * geometry)
{
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

    Shader s = Shader();
    s.Compile(vCode, fCode, gCode == nullptr ? nullptr : gCode);

    // Hash ID from file name + place new shader in container.
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(name));
    m_shaders[ID] = std::make_unique<Shader>(s);
    return ID;

}

const Texture& ResourceManager::LoadTexture(std::string p, TextureType type)
{
    Texture t = Texture();

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

        t.Generate(width, height, format, data, type, p); // Generate Texture
    }
    else {
        std::cout << "Here is the error: " << stbi_failure_reason() << '\n';
        std::cout << "Texture failed to load at path: " << p << '\n';
    }

    stbi_image_free(data);

    // Hash ID fron file name + place new Texture in container.
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(p));
    m_textures[ID] = std::make_unique<Texture>(t);
    return *m_textures[ID];
}

template<typename T>
const UniformBuffer<T>& ResourceManager::loadUniformBuffer(std::string_view path)
{
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(path));
    m_ubos[ID] = std::make_any<UniformBuffer<T>>();
    return *m_ubos[ID];
}

const uint32_t ResourceManager::LoadModel(std::string_view path)
{
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(path));
    if(m_models.find(ID) == m_models.end())
         loadModel(path);
    return ID;
}

const LoadedModel& ResourceManager::GetModel(uint32_t ID)
{
    return *m_models[ID];
    //return LoadedModel();
}

const Shader& ResourceManager::GetShader(uint32_t ID)
{
    return *m_shaders[ID];
    //return Shader();
}

const Texture& ResourceManager::GetTexture(std::string_view path)
{
    uint32_t ID = static_cast<uint32_t>(std::hash<std::string_view>{}(path));
    return *m_textures[ID];
}

template<typename T>
const UniformBuffer<T>& ResourceManager::getUniformBuffer(uint32_t ID)
{
    //return *m_ubos[ID].get();
    return NULL;
}