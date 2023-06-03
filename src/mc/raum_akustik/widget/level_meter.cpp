#include "level_meter.hpp"

#include <juce_dsp/juce_dsp.h>

namespace mc
{

LevelMeter::LevelMeter() { startTimerHz(30); }

auto LevelMeter::paint(juce::Graphics& g) -> void
{
    auto const area  = getLocalBounds().toFloat();
    auto const dBToY = [area](auto dB) { return juce::jmap(dB, -96.0F, 0.0F, area.getBottom(), area.getY()); };

    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white.withAlpha(0.25F));

    for (auto i{6}; i < 96; i += 6)
    {
        auto const y = juce::roundToInt(dBToY(static_cast<float>(-i)));
        g.drawHorizontalLine(y, area.getX(), area.getRight());
    }

    g.setColour(juce::Colours::white.withAlpha(0.75F));
    g.fillRect(area.withTop(dBToY(juce::Decibels::gainToDecibels(_peak.load()))));
}

auto LevelMeter::resized() -> void {}
auto LevelMeter::timerCallback() -> void { repaint(); }

auto LevelMeter::audioDeviceAboutToStart(juce::AudioIODevice* device) -> void
{
    auto const maxNumChannels = device->getActiveInputChannels().countNumberOfSetBits();
    auto const sampleRate     = device->getCurrentSampleRate();
    _rmsBuffer.setSize(maxNumChannels, juce::roundToInt(0.3 * sampleRate), false, true);
    _peak.store(0.0F);
    _rms.store(0.0F);
}

auto LevelMeter::audioDeviceStopped() -> void
{
    _peak.store(0.0F);
    _rms.store(0.0F);
}

auto LevelMeter::audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                                  float* const* outputChannelData, int numOutputChannels,
                                                  int numberOfSamples,
                                                  juce::AudioIODeviceCallbackContext const& context) -> void
{
    juce::ignoreUnused(context);

    auto const input = juce::dsp::AudioBlock<float const>{
        inputChannelData,
        static_cast<size_t>(numInputChannels),
        static_cast<size_t>(numberOfSamples),
    };

    auto const output = juce::dsp::AudioBlock<float>{
        outputChannelData,
        static_cast<size_t>(numOutputChannels),
        static_cast<size_t>(numberOfSamples),
    };

    auto const minmax = input.findMinAndMax();
    _peak.store(std::max(std::abs(minmax.getStart()), std::abs(minmax.getEnd())));

    output.fill(0.0F);
}

}  // namespace mc
