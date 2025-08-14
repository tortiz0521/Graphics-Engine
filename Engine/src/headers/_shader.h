#ifndef _SHADER_H
#define _SHADER_H
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    // Create and use.
    Shader() = default;
    const Shader& Use() const;

    // Clean-up shader program.
    ~Shader();

    // Shader member functions
    // A single use function that just handles the process of creating a shader programming and sending it to the GPU.
    void Compile(const char* vSource, const char *fSource, const char *gSource = nullptr);
    
    // Set shader uniforms.
    void SetFloat(const char *name, float value, bool useShader = true) const;
    void SetInteger(const char *name, int value, bool useShader = true) const;
    void SetVector2f(const char *name, float x, float y, bool useShader = true) const;
    void SetVector2f(const char *name, const glm::vec2 &value, bool useShader = true) const;
    void SetVector3f(const char *name, float x, float y, float z, bool useShader = true) const;
    void SetVector3f(const char *name, const glm::vec3 &value, bool useShader = true) const;
    void SetVector4f(const char *name, float x, float y, float z, float w, bool useShader = true) const;
    void SetVector4f(const char *name, const glm::vec4 &value, bool useShader = true) const;
    void SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader = true) const;

    void ID();

private:
    // Shader ID
    unsigned int _ID;

    // Check for any errors during 
    void checkCompilerError(unsigned int object, std::string type);
};

#endif