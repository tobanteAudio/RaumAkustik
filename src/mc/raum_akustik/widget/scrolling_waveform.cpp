#include "scrolling_waveform.hpp"

namespace mc
{

ScrollingWaveform::ScrollingWaveform() : juce::AudioVisualiserComponent(1)
{
    setSamplesPerBlock(64);
    setBufferSize(2048);
}

auto ScrollingWaveform::audioDeviceAboutToStart(juce::AudioIODevice*) -> void { clear(); }

auto ScrollingWaveform::audioDeviceStopped() -> void { clear(); }

auto ScrollingWaveform::audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                                         float* const* outputChannelData, int numOutputChannels,
                                                         int numberOfSamples,
                                                         juce::AudioIODeviceCallbackContext const& context) -> void
{
    juce::ignoreUnused(context);

    for (int i = 0; i < numberOfSamples; ++i)
    {
        float inputSample = 0;

        for (int chan = 0; chan < numInputChannels; ++chan)
            if (float const* inputChannel = inputChannelData[chan])
                inputSample += inputChannel[i];  // find the sum of all the channels

        inputSample *= 10.0f;  // boost the level to make it more easily visible.

        pushSample(&inputSample, 1);
    }

    // We need to clear the output buffers before returning, in case they're full of junk..
    for (int j = 0; j < numOutputChannels; ++j)
        if (float* outputChannel = outputChannelData[j])
            juce::zeromem(outputChannel, (size_t)numberOfSamples * sizeof(float));
}

}  // namespace mc
