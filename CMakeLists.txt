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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})
include(mcConan)
include(mcCompilerOptions)
include(mcCompilerWarnings)
include(mcCodeCoverage)

find_package(mp-units REQUIRED)
find_package(Catch2 REQUIRED)
include(Catch)

add_subdirectory(3rd_party/JUCE)
set_directory_properties(PROPERTIES JUCE_COMPANY_COPYRIGHT "Copyright 2022 Modern Circuits UG (haftungsbeschränkt). All rights reserved.")
set_directory_properties(PROPERTIES JUCE_COMPANY_NAME "Modern Circuits UG (haftungsbeschränkt)")

add_library(libRaumAkustik STATIC)
target_include_directories(libRaumAkustik PUBLIC "${PROJECT_SOURCE_DIR}/Source")

target_link_libraries(libRaumAkustik
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_sources(libRaumAkustik
    PRIVATE
        "Source/mc/RaumAkustik/Air.hpp"
        "Source/mc/RaumAkustik/FirstReflection.hpp"
        "Source/mc/RaumAkustik/PorousAbsorber.cpp"
        "Source/mc/RaumAkustik/PorousAbsorber.hpp"
        "Source/mc/RaumAkustik/Utility.cpp"
        "Source/mc/RaumAkustik/Utility.hpp"
)

add_executable(libRaumAkustik_Tests)
catch_discover_tests(libRaumAkustik_Tests)
target_link_libraries(libRaumAkustik_Tests PRIVATE libRaumAkustik Catch2::Catch2WithMain)
target_sources(libRaumAkustik_Tests
    PRIVATE
        "Source/mc/RaumAkustik/Air.test.cpp"
        "Source/mc/RaumAkustik/FirstReflection.test.cpp"
        "Source/mc/RaumAkustik/PorousAbsorber.test.cpp"
        "Source/mc/RaumAkustik/Utility.test.cpp"
)

juce_add_binary_data(RaumAkustik_BinaryData
    HEADER_NAME
        "BinaryData.hpp"
    NAMESPACE
        mcbd
    SOURCES
        "Resources/png/raum_akustik_logo_black.png"

        "Resources/svg/add.svg"
        "Resources/svg/arrow_back.svg"
        "Resources/svg/arrow_forward.svg"
        "Resources/svg/delete.svg"
        "Resources/svg/edit.svg"
        "Resources/svg/folder_open.svg"
        "Resources/svg/fullscreen.svg"
        "Resources/svg/info.svg"
        "Resources/svg/insert_emoticon.svg"
        "Resources/svg/raum_akustik_logo_black.svg"
        "Resources/svg/redo.svg"
        "Resources/svg/remove.svg"
        "Resources/svg/save.svg"
        "Resources/svg/settings.svg"
        "Resources/svg/speaker.svg"
        "Resources/svg/undo.svg"
)

juce_add_gui_app(RaumAkustik
    PRODUCT_NAME "Raum Akustik"
    ICON_BIG     "Resources/png/raum_akustik_logo_black.png"
)

target_sources(RaumAkustik
    PRIVATE
        "Source/CommandIDs.hpp"
        "Source/FirstReflectionsView.cpp"
        "Source/FirstReflectionsView.hpp"
        "Source/LookAndFeel.cpp"
        "Source/LookAndFeel.hpp"
        "Source/Main.cpp"
        "Source/MainComponent.cpp"
        "Source/MainComponent.hpp"
        "Source/MenuBar.cpp"
        "Source/MenuBar.hpp"
        "Source/PorousAbsorberSimulationView.cpp"
        "Source/PorousAbsorberSimulationView.hpp"
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
        libRaumAkustik
    PUBLIC
        RaumAkustik_BinaryData
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

