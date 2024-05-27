#include "ToneGeneratorEditor.hpp"

#include "tool/PropertyComponent.hpp"
#include "utility/ValueTree.hpp"

#include <ra/generator/GlideSweep.hpp>

#include <juce_dsp/juce_dsp.h>

#include <span>

namespace ra {

namespace {
struct FrequencyAndAmplitude
{
    float frequency;
    float amplitude;
};

template<typename T>
[[nodiscard]] constexpr auto frequencyForBin(size_t windowSize, size_t index, double sampleRate) -> T
{
    return static_cast<T>(index) * static_cast<T>(sampleRate) / static_cast<T>(windowSize);
}

auto frequencyToX(float minFreq, float maxFreq, float freq, float width)
{
    auto const logMin  = std::log(minFreq);
    auto const logMax  = std::log(maxFreq);
    auto const logFreq = std::log(freq);
    auto const ratio   = (logFreq - logMin) / (logMax - logMin);
    return width * ratio;
}

auto amplitudeToY(float amplitude, juce::Rectangle<float> const bounds) -> float
{
    auto const infinity = -96.0F;
    auto const dB       = juce::Decibels::gainToDecibels(amplitude, infinity);
    return juce::jmap(dB, infinity, 0.0F, bounds.getBottom(), bounds.getY());
}

auto getFrequencyAndAmplitude(std::span<std::complex<float> const> bins, double sampleRate)
    -> std::vector<FrequencyAndAmplitude>
{
    auto result = std::vector<FrequencyAndAmplitude>{};
    result.reserve(bins.size());

    auto const fftSize = (bins.size() - 1) * 2;
    for (auto i{0UL}; i < bins.size(); ++i) {
        auto const bin       = bins[i];
        auto const frequency = frequencyForBin<float>(fftSize, i, sampleRate);
        auto const amplitude = std::abs(bin) / static_cast<float>(fftSize);
        result.emplace_back(FrequencyAndAmplitude{frequency, amplitude});
    }

    return result;
}

auto makePathFromAnalysis(std::span<FrequencyAndAmplitude const> analysis, float fs, juce::Rectangle<float> bounds)
    -> juce::Path
{
    auto const size = static_cast<int>(analysis.size());
    if (size == 0) {
        return {};
    }

    auto frequencyLess = [](auto bin, auto target) { return bin.frequency < target; };
    auto first         = std::lower_bound(begin(analysis), end(analysis), 1.0F, frequencyLess);
    if (first == end(analysis)) {
        return {};
    }

    auto p = juce::Path{};
    p.preallocateSpace(8 + size * 3);

    auto const width  = bounds.getWidth();
    auto const startY = amplitudeToY(first->amplitude, bounds);
    p.startNewSubPath(bounds.getX() + frequencyToX(1.0F, fs * 0.5F, first->frequency, width), startY);

    for (; first != end(analysis); ++first) {
        auto const y = amplitudeToY(first->amplitude, bounds);
        p.lineTo(bounds.getX() + frequencyToX(1.0F, fs * 0.5F, first->frequency, width), y);
    }

    return p;
}

[[maybe_unused]] auto toAudioBuffer(std::vector<float> const& in) -> juce::AudioBuffer<float>
{
    auto buf = juce::AudioBuffer<float>{1, static_cast<int>(in.size())};
    std::copy(in.begin(), in.end(), buf.getWritePointer(0));
    return buf;
}

[[maybe_unused]] auto
getSpectrumPath(juce::AudioBuffer<float> const& in, double fs, juce::Rectangle<int> area) -> juce::Path
{
    auto const fftSize  = static_cast<size_t>(juce::nextPowerOfTwo(in.getNumSamples()));
    auto const fftOrder = std::bit_width(fftSize) - 1;

    auto buf     = std::vector<std::complex<float>>(fftSize);
    auto* buffer = reinterpret_cast<float*>(buf.data());  // NOLINT
    std::copy(in.getReadPointer(0), in.getReadPointer(0) + in.getNumSamples(), buffer);

    auto fft = juce::dsp::FFT{fftOrder};
    fft.performRealOnlyForwardTransform(buffer, true);

    auto const* coefficients = buf.data();
    auto const numBins       = static_cast<size_t>(fft.getSize()) / 2U + 1U;
    auto const amplitudes    = getFrequencyAndAmplitude({coefficients, numBins}, fs);
    DBG("fftSize: " << fftSize << " fftOrder: " << fftOrder << " fft.getSize(): " << fft.getSize()
                    << " numBins: " << numBins);

    return makePathFromAnalysis(amplitudes, float(fs), area.toFloat());
}
}  // namespace

ToneGeneratorEditor::ToneGeneratorEditor(juce::AudioDeviceManager& deviceManager) : _recorder{deviceManager}
{
    setPropertyIfNotExist(_valueTree, "from", 20.0);
    setPropertyIfNotExist(_valueTree, "to", 20'000.0);
    setPropertyIfNotExist(_valueTree, "curve", false);
    setPropertyIfNotExist(_valueTree, "duration", 1000.0);
    setPropertyIfNotExist(_valueTree, "sampleRate", 44100.0);

    _valueTree.addListener(this);
    _thumbnail.addChangeListener(this);

    auto const sampleRates = juce::Array<juce::var>{
        juce::var{44100.0},
        juce::var{48000.0},
        juce::var{88200.0},
        juce::var{96000.0},
        juce::var{176400.0},
        juce::var{192000.0}
    };

    auto const sampleRateNames = juce::StringArray{
        "44100",
        "48000",
        "88200",
        "96000",
        "176400",
        "192000",
    };

    _sweepSpecPanel.addProperties(juce::Array<juce::PropertyComponent*>{
        makeProperty<juce::SliderPropertyComponent>(_from, "From (Hz)", 0.0, 20'000.0, 1.0),
        makeProperty<juce::SliderPropertyComponent>(_to, "To (Hz)", 0.0, 20'000.0, 1.0),
        makeProperty<juce::BooleanPropertyComponent>(_curve, "Curve", "Lin/Log"),
        makeProperty<juce::SliderPropertyComponent>(_duration, "Duration (ms)", 0.0, 20'000.0, 1.0),
        makeProperty<juce::ChoicePropertyComponent>(_sampleRate, "Sample-rate (Hz)", sampleRateNames, sampleRates),
    });
    addAndMakeVisible(_sweepSpecPanel);
    addAndMakeVisible(_recorder);
    // handleAsyncUpdate();
}

auto ToneGeneratorEditor::paint(juce::Graphics& g) -> void
{
    g.setColour(juce::Colours::black);
    g.fillRect(_thumbnailBounds);
    g.fillRect(_spectrumBounds);

    g.setColour(juce::Colours::white);
    _thumbnail.drawChannel(g, _thumbnailBounds.reduced(4), 0.0, _thumbnail.getTotalLength(), 0, 0.5F);

    auto const b       = _spectrumBounds.toFloat();
    auto const maxFreq = static_cast<float>(static_cast<double>(_sampleRate)) / 2.0F;

    for (auto const decade : {10.0F, 100.0F, 1'000.0F}) {
        auto x0 = juce::roundToInt(b.getX() + frequencyToX(1.0F, maxFreq, decade, b.getWidth()));
        g.setColour(juce::Colours::white.withAlpha(0.5F));
        g.drawVerticalLine(x0, b.getY(), b.getBottom());

        for (auto i{1}; i < 10; ++i) {
            auto const d = decade * static_cast<float>(i);
            auto const x = juce::roundToInt(b.getX() + frequencyToX(1.0F, maxFreq, d, b.getWidth()));
            g.setColour(juce::Colours::white.withAlpha(0.25F));
            g.drawVerticalLine(x, b.getY(), b.getBottom());
        }
    }

    auto x0 = juce::roundToInt(b.getX() + frequencyToX(1.0F, maxFreq, 10'000.0F, b.getWidth()));
    g.setColour(juce::Colours::white.withAlpha(0.5F));
    g.drawVerticalLine(x0, b.getY(), b.getBottom());

    for (auto i{0}; i < 90; i += 6) {
        auto const amplitude = juce::Decibels::decibelsToGain(static_cast<float>(-i));
        auto const y         = juce::roundToInt(amplitudeToY(amplitude, b));
        g.setColour(juce::Colours::white.withAlpha(0.25F));
        g.drawHorizontalLine(y, b.getX(), b.getRight());
    }

    g.setColour(juce::Colours::white);
    g.strokePath(_spectrumPath, juce::PathStrokeType{1.0F});
}

auto ToneGeneratorEditor::resized() -> void
{
    auto area = getLocalBounds();

    auto widgets = area.removeFromLeft(area.proportionOfWidth(0.5));
    _sweepSpecPanel.setBounds(widgets.removeFromTop(widgets.proportionOfHeight(0.5)));
    _recorder.setBounds(widgets);

    _thumbnailBounds = area.removeFromTop(area.proportionOfHeight(0.5));
    _spectrumBounds  = area;

    handleAsyncUpdate();
}

auto ToneGeneratorEditor::handleAsyncUpdate() -> void
{
    auto const sampleRate = double{_sampleRate};

    auto const spec = GlideSweep{
        .from       = double{_from} * si::hertz,
        .to         = double{_to} * si::hertz,
        .curve      = bool{_curve} ? GlideSweep::Curve::Logarithmic : GlideSweep::Curve::Linear,
        .duration   = double{_duration} * si::milli<si::second>,
        .sampleRate = sampleRate * si::hertz,
    };

    auto const sweep = toAudioBuffer(generate(spec));
    _thumbnailBuffer.makeCopyOf(sweep);
    _thumbnail.clear();
    _thumbnail.reset(1, sampleRate, 1);
    _thumbnail.addBlock(0, _thumbnailBuffer, 0, _thumbnailBuffer.getNumSamples());

    _spectrumPath = getSpectrumPath(sweep, sampleRate, _spectrumBounds);
}

auto ToneGeneratorEditor::changeListenerCallback(juce::ChangeBroadcaster* /*source*/) -> void { repaint(); }

auto ToneGeneratorEditor::valueTreePropertyChanged(juce::ValueTree& /*tree*/, juce::Identifier const& /*property*/)
    -> void
{
    triggerAsyncUpdate();
}

}  // namespace ra
