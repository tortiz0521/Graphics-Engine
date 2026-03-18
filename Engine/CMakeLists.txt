cmake_minimum_required(VERSION 3.15)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(Engine)

#set(BUILD_SHARED_LIBS off)

find_package(OpenGL REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(glad CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)

find_package(glaze CONFIG REQUIRED)

add_executable(Engine)

set_output(Engine)

target_sources(Engine PRIVATE
    #src/cpp/test_main.cpp
    src/cpp/_mesh.cpp
    src/cpp/_model.cpp
    src/cpp/_shader.cpp
    src/cpp/_texture.cpp
    src/cpp/camera.cpp
    src/cpp/renderer.cpp
    src/cpp/resource_manager.cpp
    src/ECS/systems/hierarchy_system.cpp
    src/ECS/systems/batching_system.cpp
    src/cpp/scene.cpp
    src/ECS/systems/camera_system.cpp
    src/ECS/systems/light_system.cpp
    src/engine_test.cpp
 "src/engine.h" "src/ECS/components/_component_defs.cpp" "src/engine.cpp" "src/_json_parser.h")


target_link_libraries(Engine PRIVATE
    OpenGL::GL
    glm::glm
    glad::glad
    glfw
    assimp::assimp
    glaze::glaze
)