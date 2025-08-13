#include "../headers/resource_manager.h"

Shader ResourceManager::LoadShader(const char *vertex, const char *fragment, const char *name, const char * geometry)
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

    Shader s;
    s.Compile(vCode, fCode, gCode == nullptr ? nullptr : gCode);
    this->shaders[name] = s;
    //this->shaders[name].Compile(vCode, fCode, gCode == nullptr ? nullptr : gCode);
    return this->shaders[name];
}

std::shared_ptr<Texture> ResourceManager::LoadTexture(std::string directory, TextureType type)
{
    Texture t;

    int width, height, nrComp;
    std::string p = directory;
    if (type != NONE) {
        if (type == DIFFUSE)
            p += "/texture_diffuse";
        else if (type == NORMAL)
            p += "/texture_normal";
        else if(type == SPECULAR)
            p += "/texture_specular";
    }

    std::cout << p.c_str() << '\n';
        
    unsigned char *data = stbi_load(p.c_str(), &width, &height, &nrComp, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrComp == 1)
            format = GL_RED;
        else if (nrComp == 3)
            format = GL_RGB;
        else if (nrComp == 4)
            format = GL_RGBA;

        t.Generate(width, height, format, data, type, p);
    }
    else {
        std::cout << "Here is the error: " << stbi_failure_reason() << '\n';
        std::cout << "Texture failed to load at path: " << p << '\n';
    }

    stbi_image_free(data);
    this->textures[p.c_str()] = std::make_shared<Texture>(t);
    return this->textures[p.c_str()];
}

void ResourceManager::LoadModel(const char *path)
{
    this->loadModel(path);
}

void ResourceManager::GetModel(const char *path)
{
    //return this->models[path];
}

Shader ResourceManager::GetShader(const char *name)
{
    return this->shaders[name];
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const char *path)
{
    return this->textures[path];
}