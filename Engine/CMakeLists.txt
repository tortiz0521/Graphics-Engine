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


target_link_libraries(Engine PRIVATE
    OpenGL::GL
    glm::glm
    glad::glad
    glfw
    assimp::assimp
)