#version 300 es

precision highp float;
precision highp int;

out vec4 FragColor;

in vec3 modelPos;
in vec3 norm;
in vec2 texCoord;

uniform vec3 viewPos;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;

    float shininess;
};

struct DirectionLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec4 pos;
    vec4 attenuationVals;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct SpotLight
{
    vec4 pos;
    vec4 direction;
    vec4 attenuation;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float cutOff;
    float outerCutOff;
};

uniform Material mat;

#define MAX_DIR_LIGHTS 5
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

layout(std140) uniform DirectionLights 
{
    DirectionLight dLights[MAX_DIR_LIGHTS];
};

layout(std140) uniform PointLights 
{
    PointLight pLights[MAX_POINT_LIGHTS];
};

layout(std140) uniform SpotLights
{
    SpotLight sLights[MAX_SPOT_LIGHTS];
};

uniform int numDir;
uniform int numPoint;
uniform int numSpot;

vec3 CalcDirLight(DirectionLight light, vec3 n, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 n, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 n, vec3 viewDir);

void main() 
{
    vec3 n = normalize(norm);
    vec3 viewDir = normalize(viewPos - modelPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < 0; i++) {
        result += CalcDirLight(dLights[i], n, viewDir);
    }

    for (int i = 0; i < 1; i++) {
        result += CalcPointLight(pLights[i], n, viewDir);
    }

    for (int i = 0; i < 0; i++) {
        result += CalcSpotLight(sLights[i], n, viewDir);
    }

    result = min(result, vec3(1.0));

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionLight light, vec3 n, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    vec3 a = light.ambient * vec3(texture(mat.texture_diffuse1, texCoord));

    float diff = max(dot(n, lightDir), 0.0);
    vec3 d = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, texCoord));

    vec3 reflectDir = reflect(-lightDir, n);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 s = light.specular * spec * vec3(texture(mat.texture_specular1, texCoord));

    return a + d + s;
}

vec3 CalcPointLight(PointLight light, vec3 n, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos.xyz - modelPos);
    float lightDistance = length(light.pos.xyz - modelPos);

    float attenuation = 1.0 / (
        light.attenuationVals.x +
        light.attenuationVals.y * lightDistance +
        light.attenuationVals.z * lightDistance * lightDistance
    );

    vec3 a = light.ambient.xyz * vec3(texture(mat.texture_diffuse1, texCoord));

    float diff = max(dot(n, lightDir), 0.0);
    vec3 d = light.diffuse.xyz * diff * vec3(texture(mat.texture_diffuse1, texCoord));

    vec3 s = vec3(0.0);
    if (diff > 0.0) {
        vec3 reflectDir = reflect(-lightDir, n);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
        s = light.specular.xyz * spec * vec3(texture(mat.texture_specular1, texCoord));
    }

    return attenuation * (a + d + s);
}

vec3 CalcSpotLight(SpotLight light, vec3 n, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos.xyz - modelPos);
    float theta = dot(lightDir, normalize(-light.direction.xyz));

    if (theta > light.outerCutOff)
    {
        float lightDistance = length(light.pos.xyz - modelPos);

        float attenuation = 1.0 / (
            light.attenuation.x +
            light.attenuation.y * lightDistance +
            light.attenuation.z * lightDistance * lightDistance
        );

        vec3 a = light.ambient.xyz * vec3(texture(mat.texture_diffuse1, texCoord));

        float diff = max(dot(n, lightDir), 0.0);
        vec3 d = light.diffuse.xyz * diff * vec3(texture(mat.texture_diffuse1, texCoord));

        vec3 s = vec3(0.0);
        if (diff > 0.0) {
            vec3 reflectDir = reflect(-lightDir, n);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
            s = light.specular.xyz * spec * vec3(texture(mat.texture_specular1, texCoord));
        }

        float intensity = clamp(
            (theta - light.outerCutOff) /
            (light.cutOff - light.outerCutOff),
            0.0,
            1.0
        );

        return attenuation * (a + intensity * (d + s));
    }

    return vec3(0.0);
}