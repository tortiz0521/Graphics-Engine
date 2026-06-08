#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNorm;

// Output vectors to fragment
out vec3 worldPos;
out vec3 norm;
out vec2 texCoord;
// out vec3 lightPos;

// Uniforms for NDC
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate the fragment position.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    norm = mat3(transpose(inverse(model))) * aNorm;
    texCoord = aTexCoord;
}