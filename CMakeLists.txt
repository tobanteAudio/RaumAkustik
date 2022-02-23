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

    # Set C++17 with no extensions enabled. Fail if the
    # standard is not supported.
    set(CMAKE_CXX_STANDARD 17)
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
endif ()

add_subdirectory(3rd_party/Catch2 EXCLUDE_FROM_ALL)
enable_testing()
include(CTest)
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/Catch2/extras)
include(Catch)


add_subdirectory(3rd_party/JUCE)
set_directory_properties(PROPERTIES JUCE_COMPANY_COPYRIGHT "Copyright 2022 Modern Circuits UG (haftungsbeschränkt). All rights reserved.")
set_directory_properties(PROPERTIES JUCE_COMPANY_NAME "Modern Circuits UG (haftungsbeschränkt)")

add_library(libRaumAkustik STATIC)
target_include_directories(libRaumAkustik PUBLIC "${PROJECT_SOURCE_DIR}/Source")
target_sources(libRaumAkustik
    PRIVATE
        Source/mc/RaumAkustik/Air.hpp
        Source/mc/RaumAkustik/FirstReflection.hpp
        Source/mc/RaumAkustik/PorousAbsorber.cpp
        Source/mc/RaumAkustik/PorousAbsorber.hpp
        Source/mc/RaumAkustik/Utility.cpp
        Source/mc/RaumAkustik/Utility.hpp
)

add_executable(libRaumAkustik_Tests)
catch_discover_tests(libRaumAkustik_Tests)
target_link_libraries(libRaumAkustik_Tests PRIVATE libRaumAkustik Catch2::Catch2WithMain)
target_sources(libRaumAkustik_Tests
    PRIVATE
        Source/mc/RaumAkustik/Air.test.cpp
        Source/mc/RaumAkustik/FirstReflection.test.cpp
        Source/mc/RaumAkustik/PorousAbsorber.test.cpp
        Source/mc/RaumAkustik/Utility.test.cpp
)

juce_add_binary_data(RaumAkustik_BinaryData
    HEADER_NAME
        "BinaryData.hpp"
    NAMESPACE
        mcbd
    SOURCES
        Resources/svg/folder_open.svg
        Resources/svg/insert_emoticon.svg
        Resources/svg/redo.svg
        Resources/svg/save.svg
        Resources/svg/speaker.svg
        Resources/svg/undo.svg
)

juce_add_gui_app(RaumAkustik
    PRODUCT_NAME "Raum Akustik"
)

target_sources(RaumAkustik
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.hpp
        Source/RoomSimulationView.cpp
        Source/RoomSimulationView.hpp
)

target_compile_definitions(RaumAkustik
    PRIVATE
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:RaumAkustik,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:RaumAkustik,JUCE_VERSION>"
)

target_link_libraries(RaumAkustik
    PRIVATE
        juce::juce_gui_extra
    PUBLIC
        libRaumAkustik
        RaumAkustik_BinaryData
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

