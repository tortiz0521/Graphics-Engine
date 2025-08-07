#include "../headers/renderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>

void Renderer::Draw(Model &m, Shader &s, const glm::vec3 &position,
        const glm::vec3 &size,const glm::vec3 color)
{
    s.Use();

    glm::mat4 model = glm::mat4(1.0f);

    // Translate
    model = glm::translate(model, position);

    // Rotate
    model = glm::rotate(model, glm::radians((float)glfwGetTime() * 15.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    // Scale
    model = glm::scale(model, size);

    s.SetMatrix4("model", model, true);

    glActiveTexture(GL_TEXTURE0);
    //m.Draw(s);
}

void BoxRenderer::Draw(Shader &s, const glm::vec3 &position,
    const glm::vec3 &size,const glm::vec3 color, GLFWwindow *curr)
{
    s.Use();

    glm::mat4 model = glm::mat4(1.0f);

    // Translate
    model = glm::translate(model, position);

    // Rotate
    //model = glm::rotate(model, glm::radians((float)glfwGetTime() * 15.0f),
    //    glm::vec3(0.0f, 1.0f, 0.0f));

    // Scale
    model = glm::scale(model, size);

    s.SetMatrix4("model", model, true);
    GLFWwindow* compare;
    if ((compare = glfwGetCurrentContext()) != curr)
        std::cout << "LMAO";

    glBindVertexArray(this->_VAO);
    std::cout << this->_VAO;
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glDrawArrays(GL_TRIANGLES, 0, 36);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glBindVertexArray(0);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
}

void BoxRenderer::initRenderer()
{
    unsigned int VBO;

    float vertices[] = {
        // Object Coord         // Tex Coord  // Normal Coord
        // Back face
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,   0.0f,  0.0f, -1.0f,// Bottom-left
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   0.0f,  0.0f, -1.0f,// top-right
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f,   0.0f,  0.0f, -1.0f,// bottom-right         
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   0.0f,  0.0f, -1.0f,// top-right
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,   0.0f,  0.0f, -1.0f,// bottom-left
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,   0.0f,  0.0f, -1.0f,// top-left
        // Front face
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   0.0f,  0.0f, 1.0f,// bottom-left
        0.5f, -0.5f,  0.5f,     1.0f, 0.0f,   0.0f,  0.0f, 1.0f,// bottom-right
        0.5f,  0.5f,  0.5f,     1.0f, 1.0f,   0.0f,  0.0f, 1.0f,// top-right
        0.5f,  0.5f,  0.5f,     1.0f, 1.0f,   0.0f,  0.0f, 1.0f,// top-right
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,   0.0f,  0.0f, 1.0f,// top-left
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   0.0f,  0.0f, 1.0f,// bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,// top-right
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,// top-left
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,// bottom-left
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,// bottom-left
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,// bottom-right
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,// top-right
        // Right face
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   1.0f,  0.0f,  0.0f,// top-left
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f,   1.0f,  0.0f,  0.0f,// bottom-right
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   1.0f,  0.0f,  0.0f,// top-right         
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f,   1.0f,  0.0f,  0.0f,// bottom-right
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   1.0f,  0.0f,  0.0f,// top-left
        0.5f, -0.5f,  0.5f,     0.0f, 0.0f,   1.0f,  0.0f,  0.0f,// bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   0.0f, -1.0f,  0.0f,// top-right
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f,   0.0f, -1.0f,  0.0f,// top-left
        0.5f, -0.5f,  0.5f,     1.0f, 0.0f,   0.0f, -1.0f,  0.0f,// bottom-left
        0.5f, -0.5f,  0.5f,     1.0f, 0.0f,   0.0f, -1.0f,  0.0f,// bottom-left
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   0.0f, -1.0f,  0.0f,// bottom-right
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   0.0f, -1.0f,  0.0f,// top-right
        // Top face
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,   0.0f,  1.0f,  0.0f,// top-left
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   0.0f,  1.0f,  0.0f,// bottom-right
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   0.0f,  1.0f,  0.0f,// top-right     
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   0.0f,  1.0f,  0.0f,//bottom-right
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,   0.0f,  1.0f,  0.0f,// top-left
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,   0.0f,  1.0f,  0.0f// bottom-left  
    };

    std::cout << glGetString(GL_VERSION) << std::endl << glGetString(GL_VENDOR) << std::endl;
    this->_VAO = 0;
    glGenVertexArrays(1, &this->_VAO);
    std::cout << this->_VAO << "\n";
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glGenBuffers(1, &VBO);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }

    glBindVertexArray(this->_VAO);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }

    // Set the object coord
    glEnableVertexAttribArray(0);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }

    
    // Set the tex coord
    glEnableVertexAttribArray(1);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }

    // Set the normal coord
    glEnableVertexAttribArray(2);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }

    glBindVertexArray(0);
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GOT AN ERROR::" << err << '\n';
    }
}