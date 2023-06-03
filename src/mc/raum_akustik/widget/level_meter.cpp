#include "level_meter.hpp"

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
    _peakLabel.setJustificationType(juce::Justification::centred);
    _peakLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    _peakLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    _range.addItemList({"-96 dB", "-90 dB", "-84 dB", "-78 dB", "-72 dB", "-66 dB", "-60 dB"}, 1);
    _range.setSelectedId(1, juce::dontSendNotification);

    _unit.addItemList({"dbFS", "dbV", "V"}, 1);
    _unit.setSelectedId(1, juce::dontSendNotification);

    _refVoltage.setRange({0.0001, 2.000}, 0.0001);
    _refVoltage.setValue(1.0, juce::dontSendNotification);

    _smooth.setRange({1.0, 20.0}, 1.0);
    _smooth.onValueChange = [this] { _smoothValue.store(static_cast<float>(_smooth.getValue())); };
    _smooth.setValue(15.0, juce::sendNotification);

    addAndMakeVisible(_peakLabel);
    addAndMakeVisible(_range);
    addAndMakeVisible(_unit);
    addAndMakeVisible(_refVoltage);
    addAndMakeVisible(_smooth);
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
        for (auto i{0}; i < juce::roundToInt(-minDB); i += 6)
        {
            auto const y = juce::roundToInt(dBToY(static_cast<float>(-i)));
            g.drawHorizontalLine(y, area.getX(), area.getRight());
        }
    }

    auto const peakDbFS  = juce::Decibels::gainToDecibels(_peak.load());
    auto const peakVolts = dBFSToVolts(peakDbFS, vRef);
    auto const peakDbV   = voltsTodBV(peakVolts, vRef);
    auto const peaks     = std::array<float, 3>{dBToY(peakDbFS), dBToY(peakDbV), voltsToY(peakVolts)};

    auto const rmsDbFS  = juce::Decibels::gainToDecibels(_rms.load());
    auto const rmsVolts = dBFSToVolts(rmsDbFS, vRef);
    auto const rmsDbV   = voltsTodBV(rmsVolts, vRef);
    auto const rms      = std::array<float, 3>{dBToY(rmsDbFS), dBToY(rmsDbV), voltsToY(rmsVolts)};

    g.setColour(juce::Colours::white.withAlpha(0.95F));
    g.fillRect(area.withHeight(4.0F).withY(peaks[unit]));

    g.setColour(juce::Colours::white.withAlpha(0.75F));
    g.fillRect(area.withTop(rms[unit]));
}

auto LevelMeter::resized() -> void
{
    auto area         = getLocalBounds();
    auto widgets      = area.removeFromBottom(area.proportionOfHeight(0.2));
    auto widgetHeight = widgets.proportionOfHeight(0.25);

    _peakLabel.setBounds(area.removeFromTop(area.proportionOfHeight(0.075)));
    _range.setBounds(widgets.removeFromTop(widgetHeight));
    _unit.setBounds(widgets.removeFromTop(widgetHeight));
    _refVoltage.setBounds(widgets.removeFromTop(widgetHeight));
    _smooth.setBounds(widgets);

    _meter = area.toFloat();
}

auto LevelMeter::timerCallback() -> void
{
    if (_unit.getSelectedId() < 3)
    {
        auto const peakDB = juce::Decibels::gainToDecibels(_peak.load());
        auto const label  = juce::String(peakDB, 2) + " " + _unit.getText();
        _peakLabel.setText(label, juce::sendNotification);
    }
    else
    {
        auto const label = juce::String(_peak.load(), 2) + " " + _unit.getText();
        _peakLabel.setText(label, juce::sendNotification);
    }

    repaint();
}

auto LevelMeter::audioDeviceAboutToStart(juce::AudioIODevice* device) -> void
{
    auto const maxNumChannels = device->getActiveInputChannels().countNumberOfSetBits();
    auto const blockSize      = device->getCurrentBufferSizeSamples();
    auto const sampleRate     = device->getCurrentSampleRate();
    auto const blockRate      = sampleRate / blockSize;

    if (maxNumChannels == 0) { return; }

    _rmsBuffer.setSize(maxNumChannels, juce::roundToInt(0.3 * sampleRate), false, true);
    _peakFilter.prepare({blockRate, 1, 1});
    _peakFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    _peakFilter.setCutoffFrequency(15.0F);
    _peak.store(0.0F);
    _rms.store(0.0F);
    _writePosition = 0;
}

auto LevelMeter::audioDeviceStopped() -> void
{
    _peak.store(0.0F);
    _rms.store(0.0F);
    _peakFilter.reset();
}

auto LevelMeter::audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                                  float* const* outputChannelData, int numOutputChannels,
                                                  int numberOfSamples,
                                                  juce::AudioIODeviceCallbackContext const& context) -> void
{
    juce::ignoreUnused(context);

    if (numInputChannels == 0) { return; }

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
    auto const peak   = std::max(std::abs(minmax.getStart()), std::abs(minmax.getEnd()));
    _peakFilter.setCutoffFrequency(_smoothValue.load());
    _peak.store(_peakFilter.processSample(0, peak));

    for (auto i{0}; i < numberOfSamples; ++i)
    {
        for (auto channel{0}; channel < numInputChannels; ++channel)
        {
            _rmsBuffer.setSample(channel, _writePosition, inputChannelData[channel][i]);
        }

        _writePosition = (_writePosition + 1) % numberOfSamples;
    }

    _rms.store(_rmsBuffer.getRMSLevel(0, 0, _rmsBuffer.getNumSamples()));

    output.fill(0.0F);
}

}  // namespace mc
