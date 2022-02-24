#include "LookAndFeel.hpp"

namespace mc
{
LookAndFeel::LookAndFeel()
    : juce::LookAndFeel_V4 {{
        juce::Colour::fromRGB(51, 65, 85),     // windowBackground
        juce::Colour::fromRGB(30, 41, 59),     // widgetBackground
        juce::Colour::fromRGB(30, 41, 59),     // menuBackground
        juce::Colours::transparentBlack,       // outline
        juce::Colour::fromRGB(248, 250, 252),  // defaultText
        juce::Colour::fromRGB(15, 23, 42),     // defaultFill
        juce::Colour::fromRGB(248, 250, 252),  // highlightedText
        juce::Colour::fromRGB(234, 88, 12),    // highlightedFill
        juce::Colour::fromRGB(248, 250, 252),  // menuText
    }}
{
    setColour(juce::TabbedButtonBar::tabOutlineColourId, juce::Colours::transparentBlack);
}

auto LookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height) -> void
{
    g.fillAll(findColour(juce::PopupMenu::backgroundColourId));
    juce::ignoreUnused(width, height);

#if !JUCE_MAC
    // g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(0.6f));
    // g.drawRect(0, 0, width, height);
#endif
}
}  // namespace mc
