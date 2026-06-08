#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <variant>
#include <iostream>


struct DirectionLight {
    glm::vec4 direction{};
    glm::vec4 ambient{};
    glm::vec4 diffuse{};
    glm::vec4 specular{};
};

struct PointLight {
    glm::vec4 pos{};
    glm::vec4 attenuation{};
    glm::vec4 ambient{};
    glm::vec4 diffuse{};
    glm::vec4 specular{};
};

struct SpotLight {
    glm::vec4 pos{};
    glm::vec4 direction{};
    glm::vec4 attenuation{};
    //float constant;
    //float linear;
    //float quadratic;
    glm::vec4 ambient{};
    glm::vec4 diffuse{};
    glm::vec4 specular{};
    float intCut;
    float outCut;
};

typedef std::variant<DirectionLight, PointLight, SpotLight> LightVariant;

struct CameraData {
    glm::mat4 proj;
    glm::mat4 view;
};

#endif