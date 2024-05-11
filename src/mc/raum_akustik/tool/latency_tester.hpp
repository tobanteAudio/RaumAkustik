#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ra
{

struct LatencyTester final
    : juce::AudioIODeviceCallback
    , juce::Timer
{
    explicit LatencyTester(juce::TextEditor& editorBox);

    void beginTest();
    void timerCallback() override;

    juce::String getMessageDescribingResult(int latencySamples);
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels, int numSamples,
                                          juce::AudioIODeviceCallbackContext const& context) override;

private:
    // create a test sound which consists of a series of randomly-spaced audio spikes..
    void createTestSound();

    // Searches a buffer for a set of spikes that matches those in the test sound
    int findOffsetOfSpikes(juce::AudioBuffer<float> const& buffer) const;
    int calculateLatencySamples() const;

    juce::TextEditor& resultsBox;
    juce::AudioBuffer<float> testSound, recordedSound;
    juce::Array<int> spikePositions;
    juce::CriticalSection lock;

    int playingSampleNum  = 0;
    int recordedSampleNum = -1;
    int bufferSize        = 0;
    double sampleRate     = 0.0;
    bool testIsRunning    = false;
    int deviceInputLatency, deviceOutputLatency;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatencyTester)
};

struct LatencyTesterEditor final : juce::Component
{
    explicit LatencyTesterEditor(juce::AudioDeviceManager& audioDeviceManager);
    ~LatencyTesterEditor() override;

    void startTest();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioDeviceManager& audioDeviceManager;

    std::unique_ptr<LatencyTester> latencyTester;

    juce::TextButton startTestButton{"Test Latency"};
    juce::TextEditor resultsBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatencyTesterEditor)
};

}  // namespace ra
