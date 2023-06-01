#include "audio_input_tab.hpp"

namespace mc
{

AudioInputView::AudioInputView(juce::AudioDeviceManager& deviceManager)
    : _deviceManager{deviceManager}, _deviceSelector{deviceManager, 0, 256, 0, 256, true, true, true, false}
{
    addAndMakeVisible(_deviceSelector);
}

auto AudioInputView::paint(juce::Graphics& /*g*/) -> void {}

auto AudioInputView::resized() -> void { _deviceSelector.setBounds(getLocalBounds()); }

}  // namespace mc
