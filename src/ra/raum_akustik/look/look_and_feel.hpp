#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {
struct LookAndFeel final : juce::LookAndFeel_V4
{
    LookAndFeel();
    ~LookAndFeel() override = default;

    auto drawPopupMenuBackground(juce::Graphics& g, int width, int height) -> void override;
};
}  // namespace ra
