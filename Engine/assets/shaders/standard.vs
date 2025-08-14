#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec3 modelPos;
out vec3 norm;
out vec2 texCoord;

void main() 
{
	// Computer model position once!
	vec4 mpos = model * vec4(aPos, 1.0);

	// Position of vertex in Normalized Device Coordinate space.
	gl_Position = proj * view * mpos;

	modelPos = vec3(mpos);

	// Transformed normal vectors to current model space.
	norm = mat3(transpose(inverse(model))) * aNorm;

	// Regular ole' texture coordinate.
	texCoord = aTex;
}