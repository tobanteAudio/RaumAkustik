project(libRaumAkustik)

add_library(${PROJECT_NAME} STATIC)
add_library(ModernCircuits::RaumAkustik ALIAS ${PROJECT_NAME})
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
        "mc/RaumAkustik.hpp"

        "mc/RaumAkustik/Air.cpp"
        "mc/RaumAkustik/Air.hpp"
        "mc/RaumAkustik/FEM.hpp"
        "mc/RaumAkustik/FirstReflection.hpp"
        "mc/RaumAkustik/PorousAbsorber.cpp"
        "mc/RaumAkustik/PorousAbsorber.hpp"
        "mc/RaumAkustik/Units.hpp"

        "mc/RaumAkustik/Json/Source.cpp"

        "mc/RaumAkustik/Units/Density.hpp"
        "mc/RaumAkustik/Units/Frequency.cpp"
        "mc/RaumAkustik/Units/Frequency.hpp"
        "mc/RaumAkustik/Units/HeatCapacity.hpp"
        "mc/RaumAkustik/Units/Pressure.hpp"
        "mc/RaumAkustik/Units/Speed.hpp"
        "mc/RaumAkustik/Units/Temperature.hpp"
)



add_executable("${PROJECT_NAME}_Tests")
catch_discover_tests("${PROJECT_NAME}_Tests")
target_link_libraries("${PROJECT_NAME}_Tests" PRIVATE ModernCircuits::RaumAkustik Catch2::Catch2WithMain)
target_sources("${PROJECT_NAME}_Tests"
    PRIVATE
        "mc/RaumAkustik/Air.test.cpp"
        "mc/RaumAkustik/FEM.test.cpp"
        "mc/RaumAkustik/FirstReflection.test.cpp"
        "mc/RaumAkustik/PorousAbsorber.test.cpp"
        "mc/RaumAkustik/Units/Frequency.test.cpp"
)
