#include "level_meter.hpp"

#include <juce_dsp/juce_dsp.h>

namespace mc
{

// V = Vmax * 10^(dBFS / 20)
template<typename Float>
[[nodiscard]] static auto dBFSToVolts(Float dbFS, Float vMax) noexcept -> Float
{
    return vMax * std::pow(Float(10), dbFS / Float(20));
}

// dBV = 20 * log10(V / Vref)
template<typename Float>
[[nodiscard]] static auto voltsTodBV(Float volts, Float vRef) noexcept -> Float
{
    return Float(20) * std::log10(volts / vRef);
}

LevelMeter::LevelMeter()
{
    _range.addItemList({"-96 dB", "-90 dB", "-84 dB", "-78 dB", "-72 dB", "-66 dB", "-60 dB"}, 1);
    _range.setSelectedId(1, juce::dontSendNotification);

    _unit.addItemList({"dbFS", "dbV", "V"}, 1);
    _unit.setSelectedId(1, juce::dontSendNotification);

    _refVoltage.setRange({0.0001, 2.000}, 0.0001);
    _refVoltage.setValue(1.0, juce::dontSendNotification);

    addAndMakeVisible(_range);
    addAndMakeVisible(_unit);
    addAndMakeVisible(_refVoltage);
    startTimerHz(30);
}

auto LevelMeter::paint(juce::Graphics& g) -> void
{
    auto const area = _meter;

    auto const minDB = static_cast<float>(-96 + 6 * (_range.getSelectedId() - 1));
    auto const vRef  = static_cast<float>(_refVoltage.getValue());

    auto const dBToY = [area, minDB](float dB) { return juce::jmap(dB, minDB, 0.0F, area.getBottom(), area.getY()); };
    auto const voltsToY = [area, vRef](float v) { return juce::jmap(v, 0.0F, vRef, area.getBottom(), area.getY()); };

    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white.withAlpha(0.25F));

    auto const unit = _unit.getSelectedId() - 1;

    if (unit < 2)
    {
        for (auto i{6}; i < juce::roundToInt(-minDB); i += 6)
        {
            auto const y = juce::roundToInt(dBToY(static_cast<float>(-i)));
            g.drawHorizontalLine(y, area.getX(), area.getRight());
        }
    }

    auto const dbFS    = juce::Decibels::gainToDecibels(_peak.load());
    auto const volts   = dBFSToVolts(dbFS, vRef);
    auto const dbV     = voltsTodBV(volts, vRef);
    auto const options = std::array<float, 3>{dBToY(dbFS), dBToY(dbV), voltsToY(volts)};
    DBG(options[2]);

    g.setColour(juce::Colours::white.withAlpha(0.75F));
    g.fillRect(area.withTop(options[unit]));
}

auto LevelMeter::resized() -> void
{
    auto area         = getLocalBounds();
    auto widgets      = area.removeFromBottom(area.proportionOfHeight(0.15));
    auto widgetHeight = widgets.proportionOfHeight(0.33333);

    _range.setBounds(widgets.removeFromTop(widgetHeight));
    _unit.setBounds(widgets.removeFromTop(widgetHeight));
    _refVoltage.setBounds(widgets);

    _meter = area.toFloat();
}

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
