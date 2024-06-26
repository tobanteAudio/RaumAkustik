#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

#include <cstddef>

namespace ra {

struct Spectogram
    : juce::Component
    , juce::Timer
    , juce::AudioIODeviceCallback
{
    Spectogram();
    ~Spectogram() override = default;

    void audioDeviceAboutToStart(juce::AudioIODevice* /*device*/) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallbackWithContext(
        float const* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numberOfSamples,
        juce::AudioIODeviceCallbackContext const& context
    ) override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextLineOfSpectrogram();

private:
    enum
    {
        fftOrder = 11,
        fftSize  = int(1U << static_cast<unsigned>(fftOrder)),
        hopSize  = fftSize / 4,
    };

    juce::Image _image{juce::Image::RGB, 512, 512, true};

    juce::dsp::FFT _fft{fftOrder};
    juce::dsp::WindowingFunction<float> _window{fftSize, juce::dsp::WindowingFunction<float>::hamming};

    int _fifoIndex{0};
    std::atomic<bool> _nextBlockReady{false};
    std::array<float, static_cast<size_t>(fftSize) * 2UL> _fftBuffer{};
    std::array<float, fftSize> _fifoBuffer{};
};

}  // namespace ra
