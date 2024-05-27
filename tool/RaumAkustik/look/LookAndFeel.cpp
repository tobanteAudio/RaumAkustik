#include "LookAndFeel.hpp"

namespace ra {

LookAndFeel::LookAndFeel() : juce::LookAndFeel_V4{getDarkColorScheme()} {}

auto getDarkColorScheme() -> juce::LookAndFeel_V4::ColourScheme
{
    return {
        juce::Colour{0xFF1E1E1E},  // windowBackground
        juce::Colour{0xFF2E2E2E},  // widgetBackground
        juce::Colour{0xFF333333},  // menuBackground
        juce::Colour{0xFF444444},  // outline
        juce::Colour{0xFFFFFFFF},  // defaultText
        juce::Colour{0xFF3E3E3E},  // defaultFill
        juce::Colour{0xFF000000},  // highlightedText
        juce::Colour{0xFF007BFF},  // highlightedFill
        juce::Colour{0xFFFFFFFF},  // menuText
    };
}

auto getLightColorScheme() -> juce::LookAndFeel_V4::ColourScheme
{
    return {
        juce::Colour{0xFFF0F0F0},  // windowBackground
        juce::Colour{0xFFFFFFFF},  // widgetBackground
        juce::Colour{0xFF333333},  // menuBackground
        juce::Colour{0xFFCCCCCC},  // outline
        juce::Colour{0xFF000000},  // defaultText
        juce::Colour{0xFFE0E0E0},  // defaultFill
        juce::Colour{0xFFFFFFFF},  // highlightedText
        juce::Colour{0xFF007BFF},  // highlightedFill
        juce::Colour{0xFFFFFFFF},  // menuText
    };
}

auto getSchemeColour(juce::LookAndFeel_V4::ColourScheme::UIColour id) -> juce::Colour
{
    auto& lnf = juce::LookAndFeel::getDefaultLookAndFeel();
    if (auto* v4 = dynamic_cast<juce::LookAndFeel_V4*>(&lnf); v4 != nullptr) {
        return v4->getCurrentColourScheme().getUIColour(id);
    }

    // You need juce::LookAndFeel_V4 to be a base of the global LNF
    jassertfalse;
    return {};
}

auto getSchemeWindowBackgroundColour() -> juce::Colour
{
    return getSchemeColour(juce::LookAndFeel_V4::ColourScheme::windowBackground);
}

auto getSchemeWidgetBackgroundColour() -> juce::Colour
{
    return getSchemeColour(juce::LookAndFeel_V4::ColourScheme::widgetBackground);
}

auto getSchemeDefaultFillColour() -> juce::Colour
{
    return getSchemeColour(juce::LookAndFeel_V4::ColourScheme::defaultFill);
}

auto getSchemeDefaultTextColour() -> juce::Colour
{
    return getSchemeColour(juce::LookAndFeel_V4::ColourScheme::defaultText);
}

auto getSchemeHighlightFillColour() -> juce::Colour
{
    return getSchemeColour(juce::LookAndFeel_V4::ColourScheme::highlightedFill);
}

auto getSchemeHighlightTextColour() -> juce::Colour
{
    return getSchemeColour(juce::LookAndFeel_V4::ColourScheme::highlightedText);
}

}  // namespace ra
