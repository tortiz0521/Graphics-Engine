#include "_mesh.h"
#include <iostream>

Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<unsigned int> && indices,
    std::vector<std::shared_ptr<Texture>> && textures, unsigned int persistentVBO, float shine)
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    this->_vertices = vertices;
    this->_indices = indices;
    this->_textures = textures;
    this->_shininess = shine;

    setupMesh(persistentVBO);
}

void Mesh::DeleteMesh()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
}

void Mesh::Draw(const Shader &s, const size_t amount) const
{
    unsigned int diffNum = 1, specNum = 1, normNum = 1;
    for (unsigned int i = 0; i < this->_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string num;
        const std::string name = _textures[i]->GetType();
        if (name == "texture_diffuse")
            num = std::to_string(diffNum++);
        else if (name == "texture_normal")
            num = std::to_string(normNum++);
        else if (name == "texture_specular")
            num = std::to_string(specNum++);

        s.SetInteger(("mat." + name + num).c_str(), i, true);
        _textures[i]->Bind();
    }

    s.SetFloat("mat.shininess", _shininess, true);

    glBindVertexArray(this->_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, this->_indices.size(), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh(unsigned int persistentVBO)
{
    // The VAO!
    glBindVertexArray(this->_VAO);

    // For the VBO!
    glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
    glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);
    // For the EBO!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);
    
    // Vertex layout info:
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Normal layout info:
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // TexCoord layout info:
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    // Initialize array object
    glBindBuffer(GL_ARRAY_BUFFER, persistentVBO);
    size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}