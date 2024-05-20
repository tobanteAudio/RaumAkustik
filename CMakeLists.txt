cmake_minimum_required(VERSION 3.15)

find_program(CCACHE ccache)
if (CCACHE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif ()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(room-acoustics-dev VERSION 0.1.0)

enable_testing()
include(CTest)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CompilerOptions)
include(CompilerWarnings)
include(CodeCoverage)

find_package(Catch2 REQUIRED)
include(Catch)

option(DAWN_FETCH_DEPENDENCIES "Use fetch_dawn_dependencies.py as an alternative to using depot_tools" ON)

option(DAWN_BUILD_SAMPLES "Enables building Dawn's samples" OFF)
option(DAWN_ENABLE_DESKTOP_GL "Enable OpenGL" OFF)
option(DAWN_ENABLE_OPENGLES "Enable OpenGLES" OFF)
option(DAWN_USE_GLFW "Enables building glfw samples" OFF)

option(TINT_BUILD_CMD_TOOLS "Build the Tint command line tools" OFF)
option(TINT_BUILD_DOCS "Build documentation" OFF)
option(TINT_BUILD_TESTS "Build tests" OFF)

if (NOT APPLE)
    option(TINT_BUILD_MSL_WRITER "Build the MSL output writer" OFF)
endif()

if(WIN32)
    option(TINT_BUILD_GLSL_WRITER "Build the GLSL output writer" OFF)
    option(TINT_BUILD_GLSL_VALIDATOR "Build the GLSL output validator" OFF)
    option(TINT_BUILD_SPV_READER "Build the SPIR-V input reader" OFF)
    option(TINT_BUILD_SPV_WRITER "Build the SPIR-V output writer" OFF)
    option(TINT_BUILD_WGSL_READER "Build the WGSL input reader" ON)
    option(TINT_BUILD_WGSL_WRITER "Build the WGSL output writer" ON)
endif()

add_subdirectory(3rd_party/dawn EXCLUDE_FROM_ALL)
add_subdirectory(3rd_party/JUCE EXCLUDE_FROM_ALL)

add_subdirectory(src)
add_subdirectory(src/ra/raum_akustik)
