#include "LookAndFeel.hpp"

namespace ra {
LookAndFeel::LookAndFeel() { setColour(juce::TabbedButtonBar::tabOutlineColourId, juce::Colours::transparentBlack); }

auto LookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int /*width*/, int /*height*/) -> void
{
    g.fillAll(findColour(juce::PopupMenu::backgroundColourId));
}
}  // namespace ra
