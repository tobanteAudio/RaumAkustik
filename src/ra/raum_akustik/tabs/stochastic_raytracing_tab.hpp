#pragma once

#include <ra/acoustics/stochastic_raytracing.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {

struct StochasticRaytracingEditor final : juce::Component
{
    StochasticRaytracingEditor();
    ~StochasticRaytracingEditor() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

private:
    auto run() -> void;
    auto
    plotFrequency(juce::Graphics& g, juce::Rectangle<float> const& area, std::span<double const> histogram) -> void;

    juce::TextButton _render{"Render"};
    std::vector<juce::Rectangle<float>> _plots;

    std::optional<StochasticRaytracing::Result> _result;
    double _maxGain{0.0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StochasticRaytracingEditor)  // NOLINT
};

}  // namespace ra
