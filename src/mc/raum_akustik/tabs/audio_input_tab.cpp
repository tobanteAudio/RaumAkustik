#include "audio_input_tab.hpp"

namespace mc
{

AudioInputView::AudioInputView(juce::AudioDeviceManager& deviceManager)
    : _deviceManager{deviceManager}, _deviceSelector{deviceManager, 0, 256, 0, 256, true, true, true, false}
{
    addAndMakeVisible(_deviceSelector);
    addAndMakeVisible(_latencyTester);
    addAndMakeVisible(_levelMeter);

    _deviceManager.addAudioCallback(&_levelMeter);
}

AudioInputView::~AudioInputView() { _deviceManager.removeAudioCallback(&_levelMeter); }

auto AudioInputView::paint(juce::Graphics& /*g*/) -> void {}

auto AudioInputView::resized() -> void
{
    auto area = getLocalBounds();
    _levelMeter.setBounds(area.removeFromRight(area.proportionOfWidth(0.075)));
    _deviceSelector.setBounds(area.removeFromLeft(area.proportionOfWidth(0.5)));
    _latencyTester.setBounds(area);
}

}  // namespace mc
