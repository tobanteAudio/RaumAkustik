project(mc_acoustics)

find_package(mp-units REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(ra::acoustics ALIAS ${PROJECT_NAME})
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
        ra::compiler_options
        ra::coverage
)

target_sources(${PROJECT_NAME}
    PRIVATE
        "mc/acoustics/air.cpp"
        "mc/acoustics/air.hpp"

        "mc/acoustics/absorber/porous_absorber.cpp"
        "mc/acoustics/absorber/porous_absorber.hpp"

        "mc/acoustics/reflection/first_reflection.hpp"

        "mc/generator/oscillator.hpp"
        "mc/generator/sweep.cpp"
        "mc/generator/sweep.hpp"

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
target_link_libraries("${PROJECT_NAME}_Tests" PRIVATE ra::acoustics Catch2::Catch2WithMain)
target_sources("${PROJECT_NAME}_Tests"
    PRIVATE
        "mc/acoustics/air.test.cpp"
        "mc/acoustics/absorber/porous_absorber.test.cpp"
        "mc/acoustics/reflection/first_reflection.test.cpp"
        "mc/unit/frequency.test.cpp"
)
