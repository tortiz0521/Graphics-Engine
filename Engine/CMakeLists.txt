cmake_minimum_required(VERSION 3.10)

project(Engine)

find_package(OpenGL REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(glad CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)

add_executable(Engine)

set_output(Engine)

target_sources(Engine PRIVATE
    src/cpp/temp_main.cpp
    src/cpp/_mesh.cpp
    src/cpp/_model.cpp
    src/cpp/_shader.cpp
    src/cpp/_texture.cpp
    src/cpp/camera.cpp
    src/cpp/renderer.cpp
    src/cpp/resource_manager.cpp
)

set_property(TARGET Engine PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")


target_link_libraries(Engine PRIVATE
    OpenGL::GL
    glm::glm
    glad::glad
    glfw
    assimp::assimp
)