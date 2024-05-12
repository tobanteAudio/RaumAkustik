#include "stochastic_raytracing_tab.hpp"

#include <ra/acoustics/stochastic_raytracing.hpp>

#include <iostream>

namespace ra {

StochasticRaytracingEditor::StochasticRaytracingEditor()
{
    _render.onClick = [this] { run(); };
    addAndMakeVisible(_render);
}

auto StochasticRaytracingEditor::paint(juce::Graphics& g) -> void { juce::ignoreUnused(g); }

auto StochasticRaytracingEditor::resized() -> void
{
    auto area = getLocalBounds();
    _render.setBounds(area.removeFromBottom(100));
}

auto StochasticRaytracingEditor::run() -> void
{
    static constexpr auto const source     = Point{.x = 1.0, .y = 1.0, .z = 1.0};
    static constexpr auto const receiver   = Point{.x = 1.0, .y = 1.0, .z = 1.0};
    static constexpr auto const dimensions = RoomDimensions{
        .length = 600.0,
        .width  = 365.0,
        .height = 312.0,
    };
    static constexpr auto const absorption = RoomAbsorption{
        .front   = 0.33,
        .back    = 0.33,
        .left    = 0.33,
        .right   = 0.33,
        .ceiling = 0.33,
        .floor   = 0.33,
    };
    static constexpr auto const scattering = RoomScattering{
        .front   = 0.33,
        .back    = 0.33,
        .left    = 0.33,
        .right   = 0.33,
        .ceiling = 0.33,
        .floor   = 0.33,
    };

    auto const room = StochasticRaytracing::Room{
        .dimensions = dimensions,
        .absorption = absorption,
        .reflection = makeReflection(absorption),
        .scattering = scattering,
        .source     = source,
        .receiver   = receiver,
    };

    auto const simulation = StochasticRaytracing::Simulation{
        .frequencies      = std::vector{31.25, 62.5, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0, 16'000.0},
        .raysPerFrequency = 1'000U,
        .duration         = std::chrono::duration<double>{2.0},
        .timeStep         = std::chrono::duration<double>{0.0010},
        .radius           = 0.0875,
    };

    auto raytracer = StochasticRaytracing{room};

    auto start  = std::chrono::steady_clock::now();
    auto result = raytracer(simulation);
    auto stop   = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count() << "s\n";
}

}  // namespace ra