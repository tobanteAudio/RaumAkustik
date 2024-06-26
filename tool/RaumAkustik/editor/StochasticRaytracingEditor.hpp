#pragma once

#include "editor/RoomEditor.hpp"

#include <ra/acoustic/StochasticRaytracing.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {

struct StochasticRaytracingEditor final : juce::Component
{
    StochasticRaytracingEditor(juce::ThreadPool& threadPool, RoomEditor& roomEditor);
    ~StochasticRaytracingEditor() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

private:
    struct Plot final : juce::Component
    {
        Plot()           = default;
        ~Plot() override = default;

        auto plot(
            juce::String title,
            std::vector<double> const& data,
            quantity<isq::duration[si::second]> duration,
            double maxGain
        ) -> void;
        auto paint(juce::Graphics& g) -> void override;

    private:
        juce::String _title;
        std::vector<double> _data;
        quantity<isq::duration[si::second]> _duration{};
        double _maxGain{};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Plot)  // NOLINT
    };

    auto run() -> void;

    juce::ThreadPool& _threadPool;
    RoomEditor& _roomEditor;

    std::optional<StochasticRaytracing::Result> _result;
    double _maxGain{0.0};

    juce::Value _duration{juce::var(2.0)};
    juce::Value _rays{juce::var(10'000.0)};

    juce::PropertyPanel _properties;
    juce::TextButton _render{"Render"};
    juce::OwnedArray<Plot> _plots;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StochasticRaytracingEditor)  // NOLINT
};

}  // namespace ra
