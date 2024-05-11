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

add_subdirectory(3rd_party/JUCE EXCLUDE_FROM_ALL)
add_subdirectory(src)
add_subdirectory(src/ra/raum_akustik)
