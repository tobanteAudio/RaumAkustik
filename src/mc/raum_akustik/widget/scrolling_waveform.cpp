#include "scrolling_waveform.hpp"

#include <juce_dsp/juce_dsp.h>

namespace ra {

ScrollingWaveform::ScrollingWaveform() : juce::AudioVisualiserComponent(1)
{
    setSamplesPerBlock(64);
    setBufferSize(2048);
}

auto ScrollingWaveform::audioDeviceAboutToStart(juce::AudioIODevice*) -> void { clear(); }

auto ScrollingWaveform::audioDeviceStopped() -> void { clear(); }

auto ScrollingWaveform::audioDeviceIOCallbackWithContext(
    float const* const* inputChannelData,
    int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numberOfSamples,
    juce::AudioIODeviceCallbackContext const& context
) -> void
{
    juce::ignoreUnused(context);

    for (int i = 0; i < numberOfSamples; ++i) {
        float inputSample = 0;

        for (int chan = 0; chan < numInputChannels; ++chan) {
            if (float const* inputChannel = inputChannelData[chan]) {
                inputSample += inputChannel[i];  // find the sum of all the channels
            }
        }

        pushSample(&inputSample, 1);
    }

    auto const output = juce::dsp::AudioBlock<float>{
        outputChannelData,
        static_cast<size_t>(numOutputChannels),
        static_cast<size_t>(numberOfSamples),
    };
    output.fill(0.0F);
}

}  // namespace ra
