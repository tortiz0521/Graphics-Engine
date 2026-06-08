cmake_minimum_required(VERSION 3.15)

project(Engine)

# Making the engine a module
add_library(Engine STATIC)

target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
)

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)

if(NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)

    target_link_libraries(Engine PRIVATE
        OpenGL::GL
        glfw
    )

else()
    # WebAssembly uses WebGL (no OpenGL / GLFW)
    target_compile_definitions(Engine PRIVATE __EMSCRIPTEN__)
    # Emscripten settings
    add_compile_definitions(__EMSCRIPTEN__)

    add_link_options(

        # WebGL 2 / GLES3
        -sUSE_WEBGL2=1
        -sFULL_ES3=1
        -sMAX_WEBGL_VERSION=2

        # Memory
        -sALLOW_MEMORY_GROWTH=1

        # Debugging
        -sASSERTIONS=1

        # Filesystem
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets
    )
endif()

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)


target_sources(Engine PUBLIC
    "src/resource/_mesh.cpp"
    "src/resource/_model.cpp"
    "src/resource/_shader.cpp"
    "src/resource/_texture.cpp"

    "src/renderer/renderer.cpp"
    "src/resource/resource_manager.cpp"

    "src/ECS/systems/hierarchy_system.cpp"
    "src/ECS/systems/batching_system.cpp"
    "src/ECS/systems/camera_system.cpp"
    "src/ECS/systems/light_system.cpp"

    "src/scene/scene.cpp"

    "src/ECS/components/_component_defs.cpp"
    "src/engine.cpp"

    # Optional: public headers for IDE visibility
    "include/engine.h"
)


target_link_libraries(Engine 
    PUBLIC
        glad::glad
        assimp::assimp
        glaze::glaze
        glm::glm
)