project(ra_acoustics)

find_package(glm REQUIRED)
find_package(mp-units REQUIRED)

add_library(ra_acoustics STATIC)
add_library(ra::acoustics ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_recommended_warning_flags
    PUBLIC
        glm::glm
        mp-units::mp-units

        webgpu_cpp
        webgpu_dawn

        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
)

target_sources(${PROJECT_NAME}
    PRIVATE
        "ra/acoustics/air.cpp"
        "ra/acoustics/air.hpp"
        "ra/acoustics/room.hpp"
        "ra/acoustics/stochastic_raytracing.cpp"
        "ra/acoustics/stochastic_raytracing.hpp"

        "ra/acoustics/absorber/porous_absorber.cpp"
        "ra/acoustics/absorber/porous_absorber.hpp"

        "ra/acoustics/reflection/first_reflection.hpp"

        "ra/generator/oscillator.hpp"
        "ra/generator/sweep.cpp"
        "ra/generator/sweep.hpp"

        "ra/math/vec3.hpp"

        "ra/unit/frequency.cpp"
        "ra/unit/frequency.hpp"
        "ra/unit/pressure.hpp"
        "ra/unit/temperature.hpp"
)



add_executable("${PROJECT_NAME}_Tests")
catch_discover_tests("${PROJECT_NAME}_Tests")
target_link_libraries("${PROJECT_NAME}_Tests" PRIVATE ra::acoustics Catch2::Catch2WithMain)
target_sources("${PROJECT_NAME}_Tests"
    PRIVATE
        "ra/acoustics/air.test.cpp"
        "ra/acoustics/absorber/porous_absorber.test.cpp"
        "ra/acoustics/reflection/first_reflection.test.cpp"
        "ra/unit/frequency.test.cpp"
)
