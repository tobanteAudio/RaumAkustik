cmake_minimum_required(VERSION 3.15)
project(app-room-acoustics VERSION 0.1.0)

# Only do these if this is the main project,
# and not if it is included through add_subdirectory.
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
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

    # library settings
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(BUILD_SHARED_LIBS OFF)

    # misc
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Minimum macOS version
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")

    enable_testing()
    include(CTest)
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(mcCompilerOptions)
include(mcCompilerWarnings)
include(mcCodeCoverage)

find_package(mp-units REQUIRED)
find_package(Catch2 REQUIRED)
include(Catch)

add_subdirectory(3rd_party/JUCE EXCLUDE_FROM_ALL)
set_directory_properties(PROPERTIES JUCE_COMPANY_COPYRIGHT "Copyright 2022 Modern Circuits UG (haftungsbeschränkt). All rights reserved.")
set_directory_properties(PROPERTIES JUCE_COMPANY_NAME "Modern Circuits UG (haftungsbeschränkt)")

add_subdirectory(src/lib)
add_subdirectory(src/app)
