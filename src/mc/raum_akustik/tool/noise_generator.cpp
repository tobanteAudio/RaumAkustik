#include "noise_generator.hpp"

namespace mc
{

NoiseGenerator::NoiseGenerator()
{
    _play.setClickingTogglesState(true);
    _play.onClick = [this]
    {
        auto const isPlaying = _play.getToggleState();
        _play.setButtonText(isPlaying ? "Stop" : "Play");
        _isPlaying.store(isPlaying);
    };

    _gainSelect.addItemList({"-1 dBFS", "-3 dBFS", "-6 dBFS", "-12 dBFS"}, 1);
    _gainSelect.onChange = [this]
    {
        auto const gains = std::array<float, 4>{-1.0F, -3.0F, -6.0F, -12.0F};
        auto const idx   = static_cast<size_t>(_gainSelect.getSelectedId() - 1);
        _gain.store(juce::Decibels::decibelsToGain(gains[idx]));
    };

    _play.setToggleState(false, juce::sendNotification);
    _gainSelect.setSelectedId(4, juce::sendNotification);

    addAndMakeVisible(_play);
    addAndMakeVisible(_gainSelect);
}

auto NoiseGenerator::resized() -> void
{
    auto area = getLocalBounds();
    _gainSelect.setBounds(area.removeFromTop(area.proportionOfHeight(0.5)));
    _play.setBounds(area);
}

auto NoiseGenerator::audioDeviceAboutToStart(juce::AudioIODevice*) -> void {}

auto NoiseGenerator::audioDeviceStopped() -> void {}

auto NoiseGenerator::audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
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

    output.fill(0.0F);
    if (!_isPlaying.load()) { return; }

    auto* const channel = output.getChannelPointer(0);
    for (auto i{0}; i < numberOfSamples; ++i) { channel[i] = _dist(_urng) * _gain; }
}

}  // namespace mc
