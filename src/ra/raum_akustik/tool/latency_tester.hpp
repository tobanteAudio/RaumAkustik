#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {

struct LatencyTester final
    : juce::AudioIODeviceCallback
    , juce::Timer
{
    explicit LatencyTester(juce::TextEditor& editorBox);

    void beginTest();
    void timerCallback() override;

    auto getMessageDescribingResult(int latencySamples) const -> juce::String const;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallbackWithContext(
        float const* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        juce::AudioIODeviceCallbackContext const& context
    ) override;

private:
    // create a test sound which consists of a series of randomly-spaced audio spikes..
    void createTestSound();

    // Searches a buffer for a set of spikes that matches those in the test sound
    auto findOffsetOfSpikes(juce::AudioBuffer<float> const& buffer) const -> int;
    auto calculateLatencySamples() const -> int;

    juce::TextEditor& _resultsBox;
    juce::AudioBuffer<float> _testSound, _recordedSound;
    juce::Array<int> _spikePositions;
    juce::CriticalSection _lock;

    int _playingSampleNum  = 0;
    int _recordedSampleNum = -1;
    int _bufferSize        = 0;
    double _sampleRate     = 0.0;
    bool _testIsRunning    = false;
    int _deviceInputLatency{}, _deviceOutputLatency{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatencyTester)  // NOLINT
};

struct LatencyTesterEditor final : juce::Component
{
    explicit LatencyTesterEditor(juce::AudioDeviceManager& audioDeviceManager);
    ~LatencyTesterEditor() override;

    void startTest();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioDeviceManager& _audioDeviceManager;

    std::unique_ptr<LatencyTester> _latencyTester;

    juce::TextButton _startTestButton{"Test Latency"};
    juce::TextEditor _resultsBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatencyTesterEditor)  // NOLINT
};

}  // namespace ra
