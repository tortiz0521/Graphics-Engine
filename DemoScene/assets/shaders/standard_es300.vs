#version 300 es

precision highp float;
precision highp int;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 instanceMatrix;

layout(std140) uniform CameraData
{
    mat4 m_proj;
    mat4 m_view;
};

out vec3 modelPos;
out vec3 norm;
out vec2 texCoord;

void main() 
{
    vec4 mpos = instanceMatrix * vec4(aPos, 1.0);

    gl_Position = m_proj * m_view * mpos;

    modelPos = vec3(mpos);

    norm = mat3(transpose(inverse(instanceMatrix))) * aNorm;

    texCoord = aTex;
}