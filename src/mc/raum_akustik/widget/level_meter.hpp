#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

namespace ra
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

    auto audioDeviceAboutToStart(juce::AudioIODevice*) -> void override;
    auto audioDeviceStopped() -> void override;
    auto audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels, int numberOfSamples,
                                          juce::AudioIODeviceCallbackContext const& context) -> void override;

private:
    std::atomic<float> _peak{0.0F};
    std::atomic<float> _rms{0.0F};
    std::atomic<float> _smoothValue{15.0F};
    juce::AudioBuffer<float> _rmsBuffer;
    juce::dsp::StateVariableTPTFilter<float> _peakFilter{};
    int _writePosition{0};

    juce::Label _peakLabel;
    juce::ComboBox _range;
    juce::ComboBox _unit;
    juce::Slider _refVoltage{juce::Slider::IncDecButtons, juce::Slider::TextBoxRight};
    juce::Slider _smooth{juce::Slider::IncDecButtons, juce::Slider::TextBoxRight};
    juce::Rectangle<float> _meter{};
};

}  // namespace ra
