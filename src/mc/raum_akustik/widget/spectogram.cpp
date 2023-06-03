#include "spectogram.hpp"

namespace mc
{

Spectogram::Spectogram()
{
    setOpaque(true);

    startTimerHz(60);
    setSize(700, 500);
}

void Spectogram::audioDeviceAboutToStart(juce::AudioIODevice*) {}

void Spectogram::audioDeviceStopped() {}

void Spectogram::audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                                  float* const* outputChannelData, int numOutputChannels,
                                                  int numberOfSamples,
                                                  juce::AudioIODeviceCallbackContext const& context)
{
    juce::ignoreUnused(context);

    if (numInputChannels <= 0) { return; }
    for (auto i = 0; i < numberOfSamples; ++i) { pushNextSampleIntoFifo(inputChannelData[0][i]); }

    auto const output = juce::dsp::AudioBlock<float>{
        outputChannelData,
        static_cast<size_t>(numOutputChannels),
        static_cast<size_t>(numberOfSamples),
    };
    output.fill(0.0F);
}

void Spectogram::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setOpacity(1.0f);
    g.drawImage(_image, getLocalBounds().toFloat());
}

void Spectogram::timerCallback()
{
    if (_nextBlockReady.load())
    {
        drawNextLineOfSpectrogram();
        _nextBlockReady.store(false);
        repaint();
    }
}

void Spectogram::pushNextSampleIntoFifo(float sample) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next line should now be rendered..
    if (_fifoIndex == fftSize)
    {
        if (!_nextBlockReady.load())
        {
            _fftBuffer.fill(0.0F);
            std::copy(_fifoBuffer.begin(), _fifoBuffer.end(), _fftBuffer.begin());
            _nextBlockReady.store(true);
        }

        _fifoIndex = 0;
    }

    _fifoBuffer[_fifoIndex++] = sample;
}

void Spectogram::drawNextLineOfSpectrogram()
{
    auto const rightHandEdge = _image.getWidth() - 1;
    auto const imageHeight   = _image.getHeight();

    // first, shuffle our image leftwards by 1 pixel..
    _image.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

    // then render our FFT data..
    _window.multiplyWithWindowingTable(_fftBuffer.data(), fftSize);
    _fft.performFrequencyOnlyForwardTransform(_fftBuffer.data());

    // find the range of values produced, so we can scale our rendering to
    // show up the detail clearly
    auto const maxLevel = juce::FloatVectorOperations::findMinAndMax(_fftBuffer.data(), fftSize / 2);

    for (auto y = 1; y < imageHeight; ++y)
    {
        auto const skewedY = 1.0f - std::exp(std::log((float)y / (float)imageHeight) * 0.2f);
        auto const index   = juce::jlimit(0, fftSize / 2, (int)(skewedY * (int)fftSize / 2));
        auto const level   = juce::jmap(_fftBuffer[index], 0.0f, juce::jmax(maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

        _image.setPixelAt(rightHandEdge, y, juce::Colour::fromHSV(level, 1.0f, level, 1.0f));
    }
}

}  // namespace mc
