#version 330 core

out vec4 FragColor;

in vec3 modelPos;
in vec3 norm;
in vec2 texCoord;

// Position we are viewing this fragment from (for light calculations).
uniform vec3 viewPos;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;

	float shininess;
};

// Light source structs
struct DirectionLight // Think of as a sun!
{
	// Direction the light is coming from.
	vec3 direction;

	// Values for our different light calculations.
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight // Think of as a lamp of sorts.
{
	// Position of the light.
	vec3 pos;

	// Attenuation values!
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight // Think of as a flashlight.
{
	// Both the position AND direction of the light source.
	vec3 pos;
	vec3 direction;

	// Variables that allow us to make smooth/soft edges.
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material mat;

// Define the maximum amount of each type of light that a scene can have. This number is currently small for testing purposes.
#define MAX_DIR_LIGHTS 5
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

// Arrays of lights!
uniform DirectionLight dirs[MAX_DIR_LIGHTS];
uniform PointLight points[MAX_POINT_LIGHTS];
uniform SpotLight spots[MAX_SPOT_LIGHTS];

uniform int numDir;
uniform int numPoint;
uniform int numSpot;

vec3 CalcDirLight(DirectionLight light, vec3 n, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 n, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 n, vec3 viewDir);

void main() 
{
	vec3 n = normalize(norm); // Normlize the normal of this interpolated value.
	vec3 viewDir = normalize(viewPos - modelPos);

	vec3 result = vec3(0.0);

	for (int i = 0; i < numDir; i++) {
		result += CalcDirLight(dirs[i], n, viewDir);
	}

	for (int i = 0; i < numPoint; i++) {
		result += CalcPointLight(points[i], n, viewDir);
	}

	for (int i = 0; i < numSpot; i++) {
		result += CalcSpotLight(spots[i], n, viewDir);
	}

	//FragColor = texture(mat.texture_diffuse1, texCoord);
	FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionLight light, vec3 n, vec3 viewDir)
{
	// Normalize the light's direction.
	vec3 lightDir = normalize(-light.direction);

	vec3 a = light.ambient * vec3(texture(mat.texture_diffuse1, texCoord));

	float diff = max(0.0, dot(n, lightDir)); // Calculate the diffuse coefficient.
	vec3 d = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, texCoord));

	vec3 reflectDir = reflect(-lightDir, n); // Calculates the direction of the reflection on our object.
	float spec = pow(max(0.0, dot(viewDir, reflectDir)), mat.shininess); // Blinn-Phong reflection.
	vec3 s = light.specular * spec * vec3(texture(mat.texture_specular1, texCoord));

	return (a + d + s);
}

vec3 CalcPointLight(PointLight light, vec3 n, vec3 viewDir)
{
	vec3 lightDir = normalize(light.pos - modelPos);
	float lightDistance = length(light.pos - modelPos);
	float attenuation = 1.0 / (
		light.constant + (light.linear * lightDistance) + (light.quadratic * lightDistance * lightDistance)
	);
	
	vec3 a = light.ambient * vec3(texture(mat.texture_diffuse1, texCoord));

	float diff = max(0.0, dot(n, lightDir));
	vec3 d = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, texCoord));

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(0.0, dot(viewDir, reflectDir)), mat.shininess);
	vec3 s = light.specular * spec * vec3(texture(mat.texture_specular1, texCoord));

	return attenuation * (a + d + s);
}

vec3 CalcSpotLight(SpotLight light, vec3 n, vec3 viewDir)
{
	vec3 lightDir = normalize(light.pos - modelPos);
	float theta = dot(lightDir, normalize(-light.direction));

	if (theta > light.outerCutOff) {

		float lightDistance = length(light.pos - modelPos);
		float attenuation = 1.0 / (
			light.constant + (light.linear * lightDistance) + (light.quadratic * lightDistance * lightDistance)
		);

		vec3 a = light.ambient * vec3(texture(mat.texture_diffuse1, texCoord));

		float diff = max(0.0, dot(n, lightDir));
		vec3 d = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, texCoord));

		vec3 reflectDir = reflect(n, lightDir);
		float spec = pow(max(0.0, dot(viewDir, lightDir)), mat.shininess);
		vec3 s = light.specular * spec * vec3(texture(mat.texture_specular1, texCoord));

																// EPSILON
		float intensity = clamp((theta - light.outerCutOff) / (light.cutOff - light.outerCutOff), 0.0, 1.0);

		return attenuation * (a + (intensity * (d + s)));
	}
	else {
		return vec3(0.0);
	}
}