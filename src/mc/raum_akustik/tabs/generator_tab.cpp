#include "generator_tab.hpp"

#include <mc/generator/sweep.hpp>

namespace mc
{

static auto toAudioBuffer(std::vector<float> const& in) -> juce::AudioBuffer<float>
{
    auto buf = juce::AudioBuffer<float>{1, static_cast<int>(in.size())};
    std::copy(in.begin(), in.end(), buf.getWritePointer(0));
    return buf;
}

GeneratorTab::GeneratorTab()
{
    _valueTree.setProperty("from", 20.0, nullptr);
    _valueTree.setProperty("to", 20'000.0, nullptr);
    _valueTree.setProperty("curve", false, nullptr);
    _valueTree.setProperty("duration", 1000.0, nullptr);
    _valueTree.setProperty("sampleRate", 44100.0, nullptr);

    _valueTree.addListener(this);
    _thumbnail.addChangeListener(this);

    _sweepSpecPanel.addProperties(juce::Array<juce::PropertyComponent*>{
        new juce::SliderPropertyComponent{_from.getPropertyAsValue(), "From (Hz)", 0.0, 20'000.0, 1.0},
        new juce::SliderPropertyComponent{_to.getPropertyAsValue(), "To (Hz)", 0.0, 20'000.0, 1.0},
        new juce::BooleanPropertyComponent{_curve.getPropertyAsValue(), "Curve", "Lin/Log"},
        new juce::SliderPropertyComponent{_duration.getPropertyAsValue(), "Duration (ms)", 0.0, 20'000.0, 1.0},
        new juce::ChoicePropertyComponent{_sampleRate.getPropertyAsValue(), "Sample-rate (Hz)",
                                          juce::StringArray{"44100", "48000", "88200", "96000", "176400", "192000"},
                                          juce::Array<juce::var>{juce::var{44100.0}, juce::var{48000.0},
                                                                 juce::var{88200.0}, juce::var{96000.0},
                                                                 juce::var{176400.0}, juce::var{192000.0}}},
    });
    addAndMakeVisible(_sweepSpecPanel);
    handleAsyncUpdate();
}

auto GeneratorTab::paint(juce::Graphics& g) -> void
{
    g.setColour(juce::Colours::black);
    g.fillRect(_thumbnailBounds);

    g.setColour(juce::Colours::white);
    _thumbnail.drawChannel(g, _thumbnailBounds, 0.0, _thumbnail.getTotalLength(), 0, 0.5F);
}

auto GeneratorTab::resized() -> void
{
    auto area = getLocalBounds();
    _sweepSpecPanel.setBounds(area.removeFromLeft(area.proportionOfWidth(0.5)));
    _thumbnailBounds = area;
}

auto GeneratorTab::handleAsyncUpdate() -> void
{
    auto const spec = SineSweep{
        .from       = Hertz<double>{static_cast<double>(_from)},
        .to         = Hertz<double>{static_cast<double>(_to)},
        .curve      = static_cast<bool>(_curve) ? SineSweepCurve::Logarithmic : SineSweepCurve::Linear,
        .duration   = std::chrono::milliseconds{juce::roundToInt(static_cast<double>(_duration))},
        .sampleRate = static_cast<double>(_sampleRate),
    };

    auto const sweep = toAudioBuffer(generate(spec));
    _thumbnailBuffer.makeCopyOf(sweep);
    _thumbnail.clear();
    _thumbnail.reset(1, spec.sampleRate, 1);
    _thumbnail.addBlock(0, _thumbnailBuffer, 0, _thumbnailBuffer.getNumSamples());
}

auto GeneratorTab::changeListenerCallback(juce::ChangeBroadcaster* /*source*/) -> void { repaint(); }

auto GeneratorTab::valueTreePropertyChanged(juce::ValueTree& /*tree*/, juce::Identifier const& /*property*/) -> void
{
    triggerAsyncUpdate();
}

}  // namespace mc
