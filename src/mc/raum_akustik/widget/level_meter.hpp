#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

namespace mc
{

struct LevelMeter final
    : juce::Component
    , juce::Timer
    , juce::AudioIODeviceCallback
{
    LevelMeter();
    ~LevelMeter() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;
    auto timerCallback() -> void override;

    auto audioDeviceAboutToStart(juce::AudioIODevice*) -> void;
    auto audioDeviceStopped() -> void;
    auto audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels, int numberOfSamples,
                                          juce::AudioIODeviceCallbackContext const& context) -> void override;

private:
    std::atomic<float> _peak{0.0F};
    std::atomic<float> _rms{0.0F};
    juce::AudioBuffer<float> _rmsBuffer;

    juce::ComboBox _range;
    juce::ComboBox _unit;
    juce::Slider _refVoltage{juce::Slider::IncDecButtons, juce::Slider::TextBoxRight};
    juce::Rectangle<float> _meter{};
};

}  // namespace mc
