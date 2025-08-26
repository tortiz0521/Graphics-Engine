#include "../headers/_shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const Shader& Shader::Use() const
{
    glUseProgram(this->_ID);
    return *this;
}

Shader::~Shader()
{
    //glDeleteProgram(this->_ID);
}

void Shader::Compile(const char *vSource, const char *fSource, const char *gSource)
{
    unsigned int v, f, g;

    // Vertex shader creation
    v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vSource, NULL);
    glCompileShader(v);
    checkCompilerError(v, "VERTEX"); // Used to check if there were any error while compiling the shader.

    // Fragment shader creation
    f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fSource, NULL);
    glCompileShader(f);
    checkCompilerError(f, "FRAGMENT");

    // Geometry shader creation
    if (gSource != nullptr) {
        g = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(g, 1, &gSource, NULL);
        glCompileShader(g);
        checkCompilerError(g, "GEOMETRY");
    }

    // Create the actual program at our program ID.
    _ID = glCreateProgram();
    glAttachShader(_ID, v);
    glAttachShader(_ID, f);
    if (gSource != nullptr)
        glAttachShader(_ID, g);
    
    glLinkProgram(_ID);
    checkCompilerError(_ID, "PROGRAM");

    glDeleteShader(v);
    glDeleteShader(f);
    if (gSource != nullptr)
        glDeleteShader(g);
}

void Shader::checkCompilerError(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}


// These are basic uniform variable setters.
void Shader::SetFloat(const char *name, float value, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->_ID, name), value);
}

void Shader::SetInteger(const char *name, int value, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->_ID, name), value);
}

void Shader::SetVector2f(const char *name, float x, float y, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform2fv(glGetUniformLocation(this->_ID, name), 1, glm::value_ptr(glm::vec2(x, y)));
}

void Shader::SetVector2f(const char *name, const glm::vec2 &value, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform2fv(glGetUniformLocation(this->_ID, name), 1, glm::value_ptr(value));
}

void Shader::SetVector3f(const char *name, float x, float y, float z, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform3fv(glGetUniformLocation(this->_ID, name), 1, glm::value_ptr(glm::vec3(x, y, z)));
}

void Shader::SetVector3f(const char *name, const glm::vec3 &value, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform3fv(glGetUniformLocation(this->_ID, name), 1, glm::value_ptr(value));
}

void Shader::SetVector4f(const char *name, float x, float y, float z, float w, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform4fv(glGetUniformLocation(this->_ID, name), 1, glm::value_ptr(glm::vec4(x, y, z, w)));
}

void Shader::SetVector4f(const char *name, const glm::vec4 &value, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniform3fv(glGetUniformLocation(this->_ID, name), 1, glm::value_ptr(value));
}

void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader) const
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->_ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}