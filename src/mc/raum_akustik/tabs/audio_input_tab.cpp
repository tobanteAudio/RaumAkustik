#include "audio_input_tab.hpp"

namespace ra
{

AudioInputView::AudioInputView(juce::AudioDeviceManager& deviceManager)
    : _deviceManager{deviceManager}, _deviceSelector{deviceManager, 0, 256, 0, 256, true, true, true, false}
{
    addAndMakeVisible(_deviceSelector);
    addAndMakeVisible(_latencyTester);
    addAndMakeVisible(_spectogram);
    addAndMakeVisible(_noise);

    _deviceManager.addAudioCallback(&_spectogram);
    _deviceManager.addAudioCallback(&_noise);
}

AudioInputView::~AudioInputView()
{
    _deviceManager.removeAudioCallback(&_spectogram);
    _deviceManager.removeAudioCallback(&_noise);
}

auto AudioInputView::paint(juce::Graphics& /*g*/) -> void {}

auto AudioInputView::resized() -> void
{
    auto area = getLocalBounds();

    auto left = area.removeFromLeft(area.proportionOfWidth(0.5));
    _deviceSelector.setBounds(left.removeFromTop(left.proportionOfHeight(0.75)));
    _noise.setBounds(left);

    _spectogram.setBounds(area.removeFromTop(area.proportionOfHeight(0.5)));
    _latencyTester.setBounds(area);
}

}  // namespace ra
