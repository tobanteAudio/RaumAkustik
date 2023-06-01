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
        "mc/acoustics/Air.cpp"
        "mc/acoustics/Air.hpp"
        "mc/acoustics/FirstReflection.hpp"
        "mc/acoustics/sweep.cpp"
        "mc/acoustics/sweep.hpp"

        "mc/acoustics/absorber/porous_absorber.cpp"
        "mc/acoustics/absorber/porous_absorber.hpp"

        "mc/unit/density.hpp"
        "mc/unit/frequency.cpp"
        "mc/unit/frequency.hpp"
        "mc/unit/heat_capacity.hpp"
        "mc/unit/pressure.hpp"
        "mc/unit/speed.hpp"
        "mc/unit/temperature.hpp"
)



add_executable("${PROJECT_NAME}_Tests")
catch_discover_tests("${PROJECT_NAME}_Tests")
target_link_libraries("${PROJECT_NAME}_Tests" PRIVATE mc::acoustics Catch2::Catch2WithMain)
target_sources("${PROJECT_NAME}_Tests"
    PRIVATE
        "mc/acoustics/Air.test.cpp"
        "mc/acoustics/FirstReflection.test.cpp"
        "mc/acoustics/absorber/porous_absorber.test.cpp"
        "mc/unit/frequency.test.cpp"
)
