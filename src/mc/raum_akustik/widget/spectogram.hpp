#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

namespace mc
{

struct Spectogram
    : juce::Component
    , juce::Timer
    , juce::AudioIODeviceCallback
{
    Spectogram();
    ~Spectogram() override = default;

    void audioDeviceAboutToStart(juce::AudioIODevice*) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels, int numberOfSamples,
                                          juce::AudioIODeviceCallbackContext const& context) override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextLineOfSpectrogram();

private:
    enum
    {
        fftOrder = 10,
        fftSize  = 1 << fftOrder
    };

    juce::Image _image{juce::Image::RGB, 512, 512, true};

    juce::dsp::FFT _fft{fftOrder};
    juce::dsp::WindowingFunction<float> _window{fftSize, juce::dsp::WindowingFunction<float>::hann};

    int _fifoIndex{0};
    std::atomic<bool> _nextBlockReady{false};
    std::array<float, fftSize * 2> _fftBuffer{};
    std::array<float, fftSize> _fifoBuffer{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spectogram)
};

}  // namespace mc