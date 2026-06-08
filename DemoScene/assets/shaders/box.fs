#version 330 core
out vec4 FragColor;

in vec3 worldPos;
in vec3 norm;
in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, texCoord);
    //FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}