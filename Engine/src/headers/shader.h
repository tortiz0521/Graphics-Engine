#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    // Create and use.
    Shader() = default;
    Shader& Use();

    // Shader member functions
    // A single use function that just handles the process of creating a shader programming and sending it to the GPU.
    void Compile(const char* vSource, const char *fSource, const char *gSource = nullptr) noexcept;
    
    void SetFloat(const char *name, float value, bool useShader = false);
    void SetInteger(const char *name, int value, bool useShader = false);
    void SetVector2f(const char *name, float x, float y, bool useShader = false);
    void SetVector2f(const char *name, const glm::vec2 &value, bool useShader = false);
    void SetVector3f(const char *name, float x, float y, float z, bool useShader = false);
    void SetVector3f(const char *name, const glm::vec3 &value, bool useShader = false);
    void SetVector4f(const char *name, float x, float y, float z, float w, bool useShader = false);
    void SetVector4f(const char *name, const glm::vec4 &value, bool useShader = false);
    void SetMatrix4 (const char *name, const glm::mat4 &matrix, bool useShader = false);

private:
    // Shader ID
    unsigned int ID;

    // Check for any errors during 
    void checkCompilerError(unsigned int object, std::string type);
};

#endif