cmake_minimum_required(VERSION 3.15)
project(app-room-acoustics VERSION 0.1.0)

# Caches build artifacts for faster builds
find_program(CCACHE ccache)
if (CCACHE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif ()

# Set C11 with no extensions enabled. Fail if the
# standard is not supported.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Set C++20 with no extensions enabled. Fail if the
# standard is not supported.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Misc settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
include(CTest)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(mcCompilerOptions)
include(mcCompilerWarnings)
include(mcCodeCoverage)

find_package(Catch2 REQUIRED)
include(Catch)

add_subdirectory(3rd_party/JUCE EXCLUDE_FROM_ALL)
set_directory_properties(PROPERTIES JUCE_COMPANY_COPYRIGHT "Copyright 2022 Modern Circuits UG (haftungsbeschränkt). All rights reserved.")
set_directory_properties(PROPERTIES JUCE_COMPANY_NAME "Modern Circuits UG (haftungsbeschränkt)")

add_subdirectory(src)
add_subdirectory(src/mc/raum_akustik)
