#include "../headers/_mesh.h"


Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<unsigned int> && indices,
    std::vector<std::shared_ptr<Texture>> && textures)
{
    this->_vertices = vertices;
    this->_indices = indices;
    this->_textures = _textures;
}

void Mesh::Draw(Shader &s)
{
    unsigned int diffNum = 1, specNum = 1, normNum = 1;
    for (unsigned int i = 0; i < this->_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string num, name =this->_textures[i].get()->GetType();
        if (name == "texture_diffuse")
            num = std::to_string(diffNum++);
        else if (name == "texture_normal")
            num = std::to_string(normNum++);
        else if (name == "texture_specular")
            num = std::to_string(specNum++);

        s.SetInteger(("material." + name + num).c_str(), i, true);
        this->_textures[i].get()->Bind();
    }

    glBindVertexArray(this->_VAO);
    glDrawElements(GL_TRIANGLES, this->_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &this->_VAO);
    glGenBuffers(1, &this->_VBO);
    glGenBuffers(1, &this->_EBO);

    // Needs to be bounds first so that the buffer data is set to the appropriate VAO.
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

    glBindVertexArray(0);

}