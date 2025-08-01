project(Engine)


# Create a static library of all of the .h/.cpp files within the Engine!
add_library(Engine STATIC
    src/cpp/main.cpp
)

target_include_directories(Engine PUBLIC src)

find_package(OpenGL REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(glad CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)

# Every project that uses the graphics engine will have these libraries available to them.
target_link_libraries(Engine PUBLIC
    OpenGL::GL
    glm::glm
    glad::glad
    glfw
    assimp::assimp
)