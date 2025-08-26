#ifndef _TEXTURE_H
#define _TEXTURE_H
#pragma once

#include <sstream>
#include <glad/glad.h>
#include <string.h>

// For different types of maps (ie: roughness), we can add new types later.
enum TextureType 
{
    NONE = 0,
    DIFFUSE,
    NORMAL,
    SPECULAR
};

// A single texture. We can generate what the texture is, then bind it to our programs.
class Texture
{
public:
    Texture();
    ~Texture();

    // Generates the texture from the given image data.
    void Generate(unsigned int w, unsigned int h, unsigned int format,
        unsigned char *data, TextureType type, std::string &path);
    // Binds the texture as the current active 'GL_TEXTURE_2D' texture object.
    void Bind() const;

    const std::string GetType() const;
    bool ComparePath(const char *o);

    bool operator==(const Texture &o);

private:
    // Member variables
    unsigned int _ID, _type;
    const char *_path;
};

#endif