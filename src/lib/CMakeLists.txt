project(mc_acoustics)

add_library(${PROJECT_NAME} STATIC)
add_library(mc::acoustics ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_recommended_warning_flags
    PUBLIC
        mp-units::mp-units
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        mc::compiler_options
        mc::coverage
)

target_sources(${PROJECT_NAME}
    PRIVATE
        "mc/acoustics.hpp"

        "mc/acoustics/Air.cpp"
        "mc/acoustics/Air.hpp"
        "mc/acoustics/FEM.hpp"
        "mc/acoustics/FirstReflection.hpp"
        "mc/acoustics/PorousAbsorber.cpp"
        "mc/acoustics/PorousAbsorber.hpp"
        "mc/acoustics/Units.hpp"

        "mc/acoustics/Units/Density.hpp"
        "mc/acoustics/Units/Frequency.cpp"
        "mc/acoustics/Units/Frequency.hpp"
        "mc/acoustics/Units/HeatCapacity.hpp"
        "mc/acoustics/Units/Pressure.hpp"
        "mc/acoustics/Units/Speed.hpp"
        "mc/acoustics/Units/Temperature.hpp"
)



add_executable("${PROJECT_NAME}_Tests")
catch_discover_tests("${PROJECT_NAME}_Tests")
target_link_libraries("${PROJECT_NAME}_Tests" PRIVATE mc::acoustics Catch2::Catch2WithMain)
target_sources("${PROJECT_NAME}_Tests"
    PRIVATE
        "mc/acoustics/Air.test.cpp"
        "mc/acoustics/FEM.test.cpp"
        "mc/acoustics/FirstReflection.test.cpp"
        "mc/acoustics/PorousAbsorber.test.cpp"
        "mc/acoustics/Units/Frequency.test.cpp"
)
