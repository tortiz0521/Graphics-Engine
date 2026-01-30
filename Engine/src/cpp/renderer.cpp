#include "../headers/renderer.h"

void Renderer::InitRenderer()
{
    // Initiazlize dynamic buffer
    glGenBuffers(1, &m_DynamicVBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_DynamicVBO);
    glBufferData(GL_ARRAY_BUFFER, 100.0f * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
}


void Renderer::DrawInstanced(uint32_t modelID, uint32_t shaderID, const std::vector<glm::mat4>& data)
{
    const LoadedModel& m = m_manager.GetModel(modelID);
    const Shader& s = m_manager.GetShader(shaderID);

    if (true)
        UpdateVBO(data);

    s.Use();

    for (Mesh mesh : m._meshes) {
        mesh.Draw(s, data.size());
    }
}

void Renderer::Draw(const LoadedModel &m, const Shader &s, const glm::vec3 &position,
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
    for (Mesh mesh : m._meshes) {
        mesh.Draw(s);
    }
}


void Renderer::UpdateVBO(const std::vector<glm::mat4>& data)
{
    // A Non-persistent Map Orphaning implementation
    size_t dataSize = sizeof(data);
    glBindBuffer(GL_ARRAY_BUFFER, m_DynamicVBO);
    auto* ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, dataSize,
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    memcpy(ptr, &data, dataSize);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void BoxRenderer::Draw(const Shader &s, const glm::vec3 &position,
    const glm::vec3 &size,const glm::vec3 color, GLFWwindow *curr)
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

    glBindVertexArray(this->_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

// Box Renderer specific functions:

BoxRenderer::BoxRenderer()
{
    glGenVertexArrays(1, &_VAO);
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


    glGenBuffers(1, &VBO);

    glBindVertexArray(this->_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Set the object coord
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    // Set the tex coord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Set the normal coord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    glBindVertexArray(0);
}