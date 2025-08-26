cmake_minimum_required(VERSION 3.15)

# Set this BEFORE creating the project!
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
#    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUTCONFIG})
#endforeach()

project(GraphicsEngine)


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
# add_subdirectory(StarterGame)