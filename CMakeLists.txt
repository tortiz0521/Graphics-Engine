cmake_minimum_required(VERSION 3.15)

project(GraphicsEngine)

message(STATUS "EMSDK_ROOT = $ENV{EMSDK_ROOT}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Boolean describing whether the value of CMAKE_CXX_STANDARD is a requirement." FORCE)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection message
message(STATUS "EMSCRIPTEN = ${EMSCRIPTEN}")

if(EMSCRIPTEN)
    message(STATUS "Configuring Emscripten/WebAssembly build")
else()
    message(STATUS "Configuring native desktop build")
endif()

# Set this BEFORE creating the project!
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    #add_compile_options(/fsanitize=address)
    #add_compile_options(/fsanitize=undefined)
    #add_link_options(/fsanitize=address)
endif()


# A function used to ensure that our runtime directory for any given piece of code exists in their subdirectory, making the process of finding assets simpler.
function(set_output target)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug
    )
endfunction()

# The game engine itself:
add_subdirectory(Engine)

# Any games to be added to the primary directory, effectively creating a module.
add_subdirectory(DemoScene)