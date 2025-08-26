#include "../headers/_texture.h"

#include <iostream>

Texture::Texture() : _path(""), _type(NONE)
{
    glGenTextures(1, &this->_ID);
}

void Texture::Generate(unsigned int w, unsigned int h, unsigned int format,
    unsigned char *data, TextureType type, std::string &path)
{
    this->_path = path.c_str();
    this->_type = type;
    //this->_ID = 0;

    // Create the texture
    //glGenTextures(1, &this->_ID);
    std::cout << _ID;
    glBindTexture(GL_TEXTURE_2D, this->_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 
        0, format, GL_UNSIGNED_BYTE, data);
    
    // Set the textures configs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps (REQUIRED to ensure that textures will load when setting mipmap filtering)
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->_ID);
}

const std::string Texture::GetType() const
{
    if (this->_type == DIFFUSE)
        return "texture_diffuse";
    else if (this->_type == NORMAL)
        return "texture_normal";
    else if (this->_type == SPECULAR)
        return "texture_specular";
}

bool Texture::ComparePath(const char *o)
{
    return strcmp(this->_path, o) ? true : false;
}

bool Texture::operator==(const Texture &o)
{
    return strcmp(this->_path, o._path) ? true : false;
}

Texture::~Texture()
{
    //glDeleteTextures(1, &this->_ID);
}