#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {

struct LookAndFeel final : juce::LookAndFeel_V4
{
    LookAndFeel();
    ~LookAndFeel() override = default;
};

[[nodiscard]] auto getDarkColorScheme() -> juce::LookAndFeel_V4::ColourScheme;
[[nodiscard]] auto getLightColorScheme() -> juce::LookAndFeel_V4::ColourScheme;

[[nodiscard]] auto getSchemeColour(juce::LookAndFeel_V4::ColourScheme::UIColour id) -> juce::Colour;
[[nodiscard]] auto getSchemeWindowBackgroundColour() -> juce::Colour;
[[nodiscard]] auto getSchemeWidgetBackgroundColour() -> juce::Colour;
[[nodiscard]] auto getSchemeDefaultFillColour() -> juce::Colour;
[[nodiscard]] auto getSchemeDefaultTextColour() -> juce::Colour;
[[nodiscard]] auto getSchemeHighlightFillColour() -> juce::Colour;
[[nodiscard]] auto getSchemeHighlightTextColour() -> juce::Colour;

}  // namespace ra
