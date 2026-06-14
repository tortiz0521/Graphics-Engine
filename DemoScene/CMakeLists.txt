project(DemoScene)

# Emscripten settings
if(EMSCRIPTEN)
    add_compile_definitions(__EMSCRIPTEN__)

    add_link_options(
        # General settings
        -sMODULARIZE=1
        -sENVIRONMENT=web
        -sEXPORT_ES6=1

        # WebGL 2 / GLES3
        -sUSE_WEBGL2=1
        -sFULL_ES3=1
        -sMAX_WEBGL_VERSION=2

        # Memory
        -sALLOW_MEMORY_GROWTH=1
        #-SALLOW_TABLE_GROWTH=1

        # Debugging
        -sASSERTIONS=2
        -sSAFE_HEAP=1
        -sSTACK_OVERFLOW_CHECK=2
        -sERROR_ON_UNDEFINED_SYMBOLS=1
        -O0 -g3
        -sOFFSCREEN_FRAMEBUFFER=1

        # Filesystem
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/../../assets
    )
endif()

add_executable(DemoScene main.cpp)

target_link_libraries(DemoScene PRIVATE Engine)

# Function call from root directory CMakeLists.txt to assign the appropriate root directory.
set_output(DemoScene)
