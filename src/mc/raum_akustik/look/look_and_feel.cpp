#include "look_and_feel.hpp"

namespace mc
{
LookAndFeel::LookAndFeel() { setColour(juce::TabbedButtonBar::tabOutlineColourId, juce::Colours::transparentBlack); }

auto LookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int /*width*/, int /*height*/) -> void
{
    g.fillAll(findColour(juce::PopupMenu::backgroundColourId));
}
}  // namespace mc
