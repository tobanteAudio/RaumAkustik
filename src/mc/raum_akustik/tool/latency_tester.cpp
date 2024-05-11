#include "latency_tester.hpp"

#include <juce_dsp/juce_dsp.h>

namespace ra
{

LatencyTester::LatencyTester(juce::TextEditor& editorBox) : resultsBox(editorBox) {}

void LatencyTester::beginTest()
{
    resultsBox.moveCaretToEnd();
    resultsBox.insertTextAtCaret(juce::newLine + juce::newLine + "Starting test..." + juce::newLine);
    resultsBox.moveCaretToEnd();

    startTimer(50);

    const juce::ScopedLock sl(lock);
    createTestSound();
    recordedSound.clear();
    playingSampleNum = recordedSampleNum = 0;
    testIsRunning                        = true;
}

void LatencyTester::timerCallback()
{
    if (testIsRunning && recordedSampleNum >= recordedSound.getNumSamples())
    {
        testIsRunning = false;
        stopTimer();

        // Test has finished, so calculate the result..
        auto latencySamples = calculateLatencySamples();

        resultsBox.moveCaretToEnd();
        resultsBox.insertTextAtCaret(getMessageDescribingResult(latencySamples));
        resultsBox.moveCaretToEnd();
    }
}

juce::String LatencyTester::getMessageDescribingResult(int latencySamples)
{
    juce::String message;

    if (latencySamples >= 0)
    {
        message << juce::newLine << "Results (" << bufferSize << "): " << juce::newLine << latencySamples
                << " samples (" << juce::String(latencySamples * 1000.0 / sampleRate, 1) << " milliseconds)"
                << juce::newLine << "The audio device reports an input latency of " << deviceInputLatency
                << " samples, output latency of " << deviceOutputLatency << " samples." << juce::newLine
                << "So the corrected latency = " << (latencySamples - deviceInputLatency - deviceOutputLatency)
                << " samples ("
                << juce::String((latencySamples - deviceInputLatency - deviceOutputLatency) * 1000.0 / sampleRate, 2)
                << " milliseconds)";
    }
    else
    {
        message << juce::newLine << "Couldn't detect the test signal!!" << juce::newLine
                << "Make sure there's no background noise that might be confusing it..";
    }

    return message;
}

void LatencyTester::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    testIsRunning    = false;
    playingSampleNum = recordedSampleNum = 0;

    bufferSize          = device->getCurrentBufferSizeSamples();
    sampleRate          = device->getCurrentSampleRate();
    deviceInputLatency  = device->getInputLatencyInSamples();
    deviceOutputLatency = device->getOutputLatencyInSamples();

    recordedSound.setSize(1, (int)(0.9 * sampleRate));
    recordedSound.clear();
}

void LatencyTester::audioDeviceStopped() {}

void LatencyTester::audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                                     float* const* outputChannelData, int numOutputChannels,
                                                     int numSamples, juce::AudioIODeviceCallbackContext const& context)
{
    ignoreUnused(context);

    const juce::ScopedLock sl(lock);

    if (!testIsRunning)
    {
        auto const output = juce::dsp::AudioBlock<float>{
            outputChannelData,
            static_cast<size_t>(numOutputChannels),
            static_cast<size_t>(numSamples),
        };

        output.fill(0.0F);
        return;
    }

    auto* recordingBuffer = recordedSound.getWritePointer(0);
    auto* playBuffer      = testSound.getReadPointer(0);

    for (int i = 0; i < numSamples; ++i)
    {
        if (recordedSampleNum < recordedSound.getNumSamples())
        {
            auto inputSamp = 0.0f;

            for (auto j = numInputChannels; --j >= 0;)
                if (inputChannelData[j] != nullptr) inputSamp += inputChannelData[j][i];

            recordingBuffer[recordedSampleNum] = inputSamp;
        }

        ++recordedSampleNum;

        auto outputSamp = (playingSampleNum < testSound.getNumSamples()) ? playBuffer[playingSampleNum] : 0.0f;

        for (auto j = numOutputChannels; --j >= 0;)
            if (outputChannelData[j] != nullptr) outputChannelData[j][i] = outputSamp;

        ++playingSampleNum;
    }
}

// create a test sound which consists of a series of randomly-spaced audio spikes..
void LatencyTester::createTestSound()
{
    auto length = ((int)sampleRate) / 4;
    testSound.setSize(1, length);
    testSound.clear();

    juce::Random rand;

    for (int i = 0; i < length; ++i)
        testSound.setSample(0, i, (rand.nextFloat() - rand.nextFloat() + rand.nextFloat() - rand.nextFloat()) * 0.06f);

    spikePositions.clear();

    int spikePos   = 0;
    int spikeDelta = 50;

    while (spikePos < length - 1)
    {
        spikePositions.add(spikePos);

        testSound.setSample(0, spikePos, 0.99f);
        testSound.setSample(0, spikePos + 1, -0.99f);

        spikePos += spikeDelta;
        spikeDelta += spikeDelta / 6 + rand.nextInt(5);
    }
}

// Searches a buffer for a set of spikes that matches those in the test sound
int LatencyTester::findOffsetOfSpikes(juce::AudioBuffer<float> const& buffer) const
{
    auto minSpikeLevel    = 5.0f;
    auto smooth           = 0.975;
    auto* s               = buffer.getReadPointer(0);
    int spikeDriftAllowed = 5;

    juce::Array<int> spikesFound;
    spikesFound.ensureStorageAllocated(100);
    auto runningAverage = 0.0;
    int lastSpike       = 0;

    for (int i = 0; i < buffer.getNumSamples() - 10; ++i)
    {
        auto samp = std::abs(s[i]);

        if (samp > runningAverage * minSpikeLevel && i > lastSpike + 20)
        {
            lastSpike = i;
            spikesFound.add(i);
        }

        runningAverage = runningAverage * smooth + (1.0 - smooth) * samp;
    }

    int bestMatch       = -1;
    auto bestNumMatches = spikePositions.size() / 3;  // the minimum number of matches required

    if (spikesFound.size() < bestNumMatches) return -1;

    for (int offsetToTest = 0; offsetToTest < buffer.getNumSamples() - 2048; ++offsetToTest)
    {
        int numMatchesHere = 0;
        int foundIndex     = 0;

        for (int refIndex = 0; refIndex < spikePositions.size(); ++refIndex)
        {
            auto referenceSpike = spikePositions.getUnchecked(refIndex) + offsetToTest;
            int spike           = 0;

            while ((spike = spikesFound.getUnchecked(foundIndex)) < referenceSpike - spikeDriftAllowed
                   && foundIndex < spikesFound.size() - 1)
                ++foundIndex;

            if (spike >= referenceSpike - spikeDriftAllowed && spike <= referenceSpike + spikeDriftAllowed)
                ++numMatchesHere;
        }

        if (numMatchesHere > bestNumMatches)
        {
            bestNumMatches = numMatchesHere;
            bestMatch      = offsetToTest;

            if (numMatchesHere == spikePositions.size()) break;
        }
    }

    return bestMatch;
}

int LatencyTester::calculateLatencySamples() const
{
    // Detect the sound in both our test sound and the recording of it, and measure the difference
    // in their start times..
    auto referenceStart = findOffsetOfSpikes(testSound);
    jassert(referenceStart >= 0);

    auto recordedStart = findOffsetOfSpikes(recordedSound);

    return (recordedStart < 0) ? -1 : (recordedStart - referenceStart);
}

LatencyTesterEditor::LatencyTesterEditor(juce::AudioDeviceManager& deviceManager)
    : audioDeviceManager{deviceManager}
{
    setOpaque(true);

    addAndMakeVisible(resultsBox);
    resultsBox.setMultiLine(true);
    resultsBox.setReturnKeyStartsNewLine(true);
    resultsBox.setReadOnly(true);
    resultsBox.setScrollbarsShown(true);
    resultsBox.setCaretVisible(false);
    resultsBox.setPopupMenuEnabled(true);

    resultsBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    resultsBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    resultsBox.setText("Running this test measures the round-trip latency between the audio output and input "
                       "devices you\'ve got selected.\n\n"
                       "It\'ll play a sound, then try to measure the time at which the sound arrives "
                       "back at the audio input. Obviously for this to work you need to have your "
                       "microphone somewhere near your speakers...");

    addAndMakeVisible(startTestButton);
    startTestButton.onClick = [this] { startTest(); };

    setSize(500, 500);
}

LatencyTesterEditor::~LatencyTesterEditor()
{
    audioDeviceManager.removeAudioCallback(latencyTester.get());
    latencyTester.reset();
}

void LatencyTesterEditor::startTest()
{
    if (latencyTester.get() == nullptr)
    {
        latencyTester.reset(new LatencyTester(resultsBox));
        audioDeviceManager.addAudioCallback(latencyTester.get());
    }

    latencyTester->beginTest();
}

void LatencyTesterEditor::paint(juce::Graphics& g) { g.fillAll(findColour(juce::ResizableWindow::backgroundColourId)); }

void LatencyTesterEditor::resized()
{
    auto b = getLocalBounds().reduced(5);
    startTestButton.setBounds(b.removeFromBottom(b.getHeight() / 10));
    b.removeFromBottom(10);
    resultsBox.setBounds(b);
}

}  // namespace ra
