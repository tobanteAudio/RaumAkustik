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
        neosonar::neo

        webgpu_cpp
        webgpu_dawn
)

target_sources(${PROJECT_NAME}
    PRIVATE
        "ra/acoustic/Air.cpp"
        "ra/acoustic/Air.hpp"
        "ra/acoustic/FirstReflection.hpp"
        "ra/acoustic/Room.hpp"
        "ra/acoustic/StochasticRaytracing.cpp"
        "ra/acoustic/StochasticRaytracing.hpp"
        "ra/acoustic/WaveEquation2D.cpp"
        "ra/acoustic/WaveEquation2D.hpp"

        "ra/acoustic/absorber/PorousAbsorber.cpp"
        "ra/acoustic/absorber/PorousAbsorber.hpp"

        "ra/generator/SineOscillator.hpp"
        "ra/generator/GlideSweep.cpp"
        "ra/generator/GlideSweep.hpp"

        "ra/geometry/Vec3.hpp"

        "ra/unit/frequency.cpp"
        "ra/unit/frequency.hpp"
        "ra/unit/pressure.hpp"
        "ra/unit/temperature.hpp"
        "ra/unit/unit.hpp"
)


add_executable("${PROJECT_NAME}_Tests")
catch_discover_tests("${PROJECT_NAME}_Tests")
target_link_libraries("${PROJECT_NAME}_Tests" PRIVATE ra::acoustics Catch2::Catch2WithMain)
target_sources("${PROJECT_NAME}_Tests"
    PRIVATE
        "ra/acoustic/Air.test.cpp"
        "ra/acoustic/FirstReflection.test.cpp"
        "ra/acoustic/absorber/PorousAbsorber.test.cpp"
        "ra/unit/frequency.test.cpp"
)
