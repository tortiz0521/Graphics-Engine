cmake_minimum_required(VERSION 3.10)
project(GraphicsEngine)

# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")

# A function used to ensure that our runtime directory for any given piece of code exists in their subdirectory, making the process of finding assets simpler.
function(set_output target)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    )
endfunction()

# The game engine itself:
add_subdirectory(Engine)

# Any games to be added to the primary directory, effectively creating a module.
# add_subdirectory(StarterGame)