#include "../headers/shader.h"

Shader& Shader::Use()
{
    glUseProgram(this->ID);
    return *this;
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
    this->ID = glCreateProgram();
    glAttachShader(this->ID, v);
    glAttachShader(this->ID, f);
    if (gSource != nullptr)
        glAttachShader(this->ID, g);
    
    glLinkProgram(this->ID);
    checkCompilerError(this->ID, "PROGRAM");
}
