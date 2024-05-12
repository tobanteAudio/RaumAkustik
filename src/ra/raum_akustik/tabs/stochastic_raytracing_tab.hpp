#pragma once

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

    juce::TextButton _render{"Render"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StochasticRaytracingEditor)  // NOLINT
};

}  // namespace ra
