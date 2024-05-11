#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

namespace ra {

struct ScrollingWaveform final
    : juce::AudioVisualiserComponent
    , juce::AudioIODeviceCallback
{
    ScrollingWaveform();
    ~ScrollingWaveform() override = default;

    auto audioDeviceAboutToStart(juce::AudioIODevice*) -> void override;
    auto audioDeviceStopped() -> void override;
    auto audioDeviceIOCallbackWithContext(
        float const* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numberOfSamples,
        juce::AudioIODeviceCallbackContext const& context
    ) -> void override;
};

}  // namespace ra
