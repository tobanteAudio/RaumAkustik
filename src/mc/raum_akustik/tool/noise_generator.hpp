#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

#include <random>

namespace ra
{

struct NoiseGenerator final
    : juce::Component
    , juce::AudioIODeviceCallback
{
    NoiseGenerator();
    ~NoiseGenerator() override = default;

    auto resized() -> void override;

    auto audioDeviceAboutToStart(juce::AudioIODevice*) -> void override;
    auto audioDeviceStopped() -> void override;
    auto audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels, int numberOfSamples,
                                          juce::AudioIODeviceCallbackContext const& context) -> void override;

private:
    std::mt19937 _urng;
    std::uniform_real_distribution<float> _dist{-1.0F, 1.0F};
    std::atomic<float> _gain{1.0F};
    std::atomic<bool> _isPlaying{false};

    juce::TextButton _play{"Play"};
    juce::ComboBox _gainSelect{"Gain"};
};

}  // namespace ra
