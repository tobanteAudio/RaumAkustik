#include "stochastic_raytracing_tab.hpp"

#include <iostream>
#include <juce_audio_basics/juce_audio_basics.h>

namespace ra {

StochasticRaytracingEditor::StochasticRaytracingEditor()
{
    _render.onClick = [this] { run(); };
    addAndMakeVisible(_render);
}

auto StochasticRaytracingEditor::paint(juce::Graphics& g) -> void
{
    g.fillAll(juce::Colours::white);

    if (not _result) {
        return;
    }

    for (auto i{0U}; i < _result->size(); ++i) {
        plotFrequency(g, _plots[i].reduced(16.0F), std::span{_result->at(i)});
    }
}

auto StochasticRaytracingEditor::resized() -> void
{
    auto area = getLocalBounds();
    _render.setBounds(area.removeFromBottom(100));
    _plots.resize(10);

    auto top   = area.removeFromTop(area.proportionOfHeight(0.5));
    auto width = area.proportionOfWidth(1.0 / 5.0);

    // top
    _plots[0] = top.removeFromLeft(width).toFloat();
    _plots[1] = top.removeFromLeft(width).toFloat();
    _plots[2] = top.removeFromLeft(width).toFloat();
    _plots[3] = top.removeFromLeft(width).toFloat();
    _plots[4] = top.removeFromLeft(width).toFloat();

    // bottom
    _plots[5] = area.removeFromLeft(width).toFloat();
    _plots[6] = area.removeFromLeft(width).toFloat();
    _plots[7] = area.removeFromLeft(width).toFloat();
    _plots[8] = area.removeFromLeft(width).toFloat();
    _plots[9] = area.removeFromLeft(width).toFloat();
}

auto StochasticRaytracingEditor::run() -> void
{
    static constexpr auto const source = Point{
        .x = 1.6,
        .y = 1.21,
        .z = 1.25,
    };
    static constexpr auto const receiver = Point{
        .x = 1.6 + 1.21,
        .y = 1.21 + 1.21 / 2.0,
        .z = 1.2,
    };
    static constexpr auto const dimensions = RoomDimensions{
        .length = 6.0,
        .width  = 3.65,
        .height = 3.12,
    };

    auto const simulation = StochasticRaytracing::Simulation{
        .frequencies      = std::vector{31.25, 62.5, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0, 16'000.0},
        .raysPerFrequency = 10'000U,
        .duration         = std::chrono::duration<double>{2.0},
        .timeStep         = std::chrono::duration<double>{0.001},
        .radius           = 0.0875,
    };

    auto const paintedConcrete = std::vector{0.01, 0.01, 0.01, 0.05, 0.06, 0.07, 0.09, 0.08, 0.08, 0.08};
    auto const woodFloor       = std::vector{0.15, 0.15, 0.15, 0.11, 0.1, 0.07, 0.06, 0.07, 0.07, 0.07};

    auto const absorption = RoomAbsorption{
        .front   = paintedConcrete,
        .back    = paintedConcrete,
        .left    = paintedConcrete,
        .right   = paintedConcrete,
        .ceiling = paintedConcrete,
        .floor   = woodFloor,
    };

    auto const scattering = RoomScattering{
        .front   = {0.05, 0.05, 0.05, 0.3,  0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .back    = {0.05, 0.05, 0.05, 0.3,  0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .left    = {0.05, 0.05, 0.05, 0.3,  0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .right   = {0.05, 0.05, 0.05, 0.3,  0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .ceiling = {0.05, 0.05, 0.05, 0.3,  0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .floor   = {0.01, 0.01, 0.01, 0.05, 0.1, 0.2, 0.3,  0.5, 0.5, 0.5},
    };

    auto const room = StochasticRaytracing::Room{
        .dimensions = dimensions,
        .absorption = absorption,
        .reflection = makeReflection(absorption),
        .scattering = scattering,
        .source     = source,
        .receiver   = receiver,
    };

    auto raytracer = StochasticRaytracing{room};

    auto start = std::chrono::steady_clock::now();
    _result    = raytracer(simulation);
    auto stop  = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count() << "s\n";

    for (auto const& frequency : *_result) {
        _maxGain = std::max(_maxGain, *std::max_element(frequency.begin(), frequency.end()));
    }

    repaint();
}

auto StochasticRaytracingEditor::plotFrequency(
    juce::Graphics& g,
    juce::Rectangle<float> const& area,
    std::span<double const> histogram
) -> void
{
    auto const gainToY = [&](auto gain) {
        static constexpr auto const minDB = -80.0F;
        static constexpr auto const maxDB = 0.0F;

        auto const normalized = static_cast<float>(gain);
        auto const dB         = juce::Decibels::gainToDecibels(normalized, minDB);
        return juce::jmap(dB, minDB, maxDB, area.getBottom(), area.getY());
    };

    // outline
    g.setColour(juce::Colours::black);
    g.drawRect(area, 1.0F);

    // grid
    // dB
    auto const dBm10 = gainToY(juce::Decibels::decibelsToGain(-10.0));
    auto const dBm20 = gainToY(juce::Decibels::decibelsToGain(-20.0));
    auto const dBm30 = gainToY(juce::Decibels::decibelsToGain(-30.0));
    auto const dBm60 = gainToY(juce::Decibels::decibelsToGain(-60.0));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm10), area.getBottomRight().withY(dBm10)));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm20), area.getBottomRight().withY(dBm20)));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm30), area.getBottomRight().withY(dBm30)));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm60), area.getBottomRight().withY(dBm60)));

    // time
    auto const sec05 = juce::roundToInt(area.getX() + area.proportionOfWidth(0.25));
    auto const sec1  = juce::roundToInt(area.getX() + area.proportionOfWidth(0.5));
    auto const sec15 = juce::roundToInt(area.getX() + area.proportionOfWidth(0.75));
    g.drawVerticalLine(sec05, area.getY(), area.getBottom());
    g.drawVerticalLine(sec1, area.getY(), area.getBottom());
    g.drawVerticalLine(sec15, area.getY(), area.getBottom());

    // plot
    auto path = juce::Path{};
    path.startNewSubPath({0.0F, gainToY(histogram[0] / _maxGain)});

    auto const deltaX = area.getWidth() / static_cast<float>(histogram.size());
    for (auto i{1U}; i < histogram.size(); ++i) {
        auto const x = area.getX() + deltaX * static_cast<float>(i);
        path.lineTo({x, gainToY(histogram[i] / _maxGain)});
    }

    path.lineTo(area.getBottomRight());

    g.setColour(juce::Colours::black);
    g.strokePath(path, juce::PathStrokeType{1.0F, juce::PathStrokeType::curved});
}

}  // namespace ra