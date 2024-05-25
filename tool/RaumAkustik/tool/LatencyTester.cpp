#include "LatencyTester.hpp"

#include <juce_dsp/juce_dsp.h>

#include <memory>

namespace ra {

LatencyTester::LatencyTester(juce::TextEditor& editorBox) : _resultsBox(editorBox) {}

void LatencyTester::beginTest()
{
    _resultsBox.moveCaretToEnd();
    _resultsBox.insertTextAtCaret(juce::newLine + juce::newLine + "Starting test..." + juce::newLine);
    _resultsBox.moveCaretToEnd();

    startTimer(50);

    juce::ScopedLock const sl(_lock);
    createTestSound();
    _recordedSound.clear();
    _playingSampleNum = _recordedSampleNum = 0;
    _testIsRunning                         = true;
}

void LatencyTester::timerCallback()
{
    if (_testIsRunning && _recordedSampleNum >= _recordedSound.getNumSamples()) {
        _testIsRunning = false;
        stopTimer();

        // Test has finished, so calculate the result..
        auto latencySamples = calculateLatencySamples();

        _resultsBox.moveCaretToEnd();
        _resultsBox.insertTextAtCaret(getMessageDescribingResult(latencySamples));
        _resultsBox.moveCaretToEnd();
    }
}

auto LatencyTester::getMessageDescribingResult(int latencySamples) const -> juce::String const
{
    juce::String message;

    if (latencySamples >= 0) {
        message << juce::newLine << "Results (" << _bufferSize << "): " << juce::newLine << latencySamples
                << " samples (" << juce::String(latencySamples * 1000.0 / _sampleRate, 1) << " milliseconds)"
                << juce::newLine << "The audio device reports an input latency of " << _deviceInputLatency
                << " samples, output latency of " << _deviceOutputLatency << " samples." << juce::newLine
                << "So the corrected latency = " << (latencySamples - _deviceInputLatency - _deviceOutputLatency)
                << " samples ("
                << juce::String((latencySamples - _deviceInputLatency - _deviceOutputLatency) * 1000.0 / _sampleRate, 2)
                << " milliseconds)";
    } else {
        message << juce::newLine << "Couldn't detect the test signal!!" << juce::newLine
                << "Make sure there's no background noise that might be confusing it..";
    }

    return message;
}

void LatencyTester::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    _testIsRunning    = false;
    _playingSampleNum = _recordedSampleNum = 0;

    _bufferSize          = device->getCurrentBufferSizeSamples();
    _sampleRate          = device->getCurrentSampleRate();
    _deviceInputLatency  = device->getInputLatencyInSamples();
    _deviceOutputLatency = device->getOutputLatencyInSamples();

    _recordedSound.setSize(1, (int)(0.9 * _sampleRate));
    _recordedSound.clear();
}

void LatencyTester::audioDeviceStopped() {}

void LatencyTester::audioDeviceIOCallbackWithContext(
    float const* const* inputChannelData,
    int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    juce::AudioIODeviceCallbackContext const& context
)
{
    ignoreUnused(context);

    juce::ScopedLock const sl(_lock);

    if (!_testIsRunning) {
        auto const output = juce::dsp::AudioBlock<float>{
            outputChannelData,
            static_cast<size_t>(numOutputChannels),
            static_cast<size_t>(numSamples),
        };

        output.fill(0.0F);
        return;
    }

    auto* recordingBuffer  = _recordedSound.getWritePointer(0);
    auto const* playBuffer = _testSound.getReadPointer(0);

    for (int i = 0; i < numSamples; ++i) {
        if (_recordedSampleNum < _recordedSound.getNumSamples()) {
            auto inputSamp = 0.0F;

            for (auto j = numInputChannels; --j >= 0;) {
                if (inputChannelData[j] != nullptr) {
                    inputSamp += inputChannelData[j][i];
                }
            }

            recordingBuffer[_recordedSampleNum] = inputSamp;
        }

        ++_recordedSampleNum;

        auto outputSamp = (_playingSampleNum < _testSound.getNumSamples()) ? playBuffer[_playingSampleNum] : 0.0F;

        for (auto j = numOutputChannels; --j >= 0;) {
            if (outputChannelData[j] != nullptr) {
                outputChannelData[j][i] = outputSamp;
            }
        }

        ++_playingSampleNum;
    }
}

// create a test sound which consists of a series of randomly-spaced audio spikes..
void LatencyTester::createTestSound()
{
    auto length = ((int)_sampleRate) / 4;
    _testSound.setSize(1, length);
    _testSound.clear();

    juce::Random rand;

    for (int i = 0; i < length; ++i) {
        _testSound.setSample(0, i, (rand.nextFloat() - rand.nextFloat() + rand.nextFloat() - rand.nextFloat()) * 0.06F);
    }

    _spikePositions.clear();

    int spikePos   = 0;
    int spikeDelta = 50;

    while (spikePos < length - 1) {
        _spikePositions.add(spikePos);

        _testSound.setSample(0, spikePos, 0.99F);
        _testSound.setSample(0, spikePos + 1, -0.99F);

        spikePos += spikeDelta;
        spikeDelta += spikeDelta / 6 + rand.nextInt(5);
    }
}

// Searches a buffer for a set of spikes that matches those in the test sound
auto LatencyTester::findOffsetOfSpikes(juce::AudioBuffer<float> const& buffer) const -> int
{
    auto minSpikeLevel          = 5.0F;
    auto smooth                 = 0.975;
    auto const* s               = buffer.getReadPointer(0);
    int const spikeDriftAllowed = 5;

    juce::Array<int> spikesFound;
    spikesFound.ensureStorageAllocated(100);
    auto runningAverage = 0.0;
    int lastSpike       = 0;

    for (int i = 0; i < buffer.getNumSamples() - 10; ++i) {
        auto samp = std::abs(s[i]);

        if (samp > runningAverage * minSpikeLevel && i > lastSpike + 20) {
            lastSpike = i;
            spikesFound.add(i);
        }

        runningAverage = runningAverage * smooth + (1.0 - smooth) * samp;
    }

    int bestMatch       = -1;
    auto bestNumMatches = _spikePositions.size() / 3;  // the minimum number of matches required

    if (spikesFound.size() < bestNumMatches) {
        return -1;
    }

    for (int offsetToTest = 0; offsetToTest < buffer.getNumSamples() - 2048; ++offsetToTest) {
        int numMatchesHere = 0;
        int foundIndex     = 0;

        for (int refIndex = 0; refIndex < _spikePositions.size(); ++refIndex) {
            auto referenceSpike = _spikePositions.getUnchecked(refIndex) + offsetToTest;
            int spike           = 0;

            while ((spike = spikesFound.getUnchecked(foundIndex)) < referenceSpike - spikeDriftAllowed
                   && foundIndex < spikesFound.size() - 1) {
                ++foundIndex;
            }

            if (spike >= referenceSpike - spikeDriftAllowed && spike <= referenceSpike + spikeDriftAllowed) {
                ++numMatchesHere;
            }
        }

        if (numMatchesHere > bestNumMatches) {
            bestNumMatches = numMatchesHere;
            bestMatch      = offsetToTest;

            if (numMatchesHere == _spikePositions.size()) {
                break;
            }
        }
    }

    return bestMatch;
}

auto LatencyTester::calculateLatencySamples() const -> int
{
    // Detect the sound in both our test sound and the recording of it, and measure the difference
    // in their start times..
    auto referenceStart = findOffsetOfSpikes(_testSound);
    jassert(referenceStart >= 0);

    auto recordedStart = findOffsetOfSpikes(_recordedSound);

    return (recordedStart < 0) ? -1 : (recordedStart - referenceStart);
}

LatencyTesterEditor::LatencyTesterEditor(juce::AudioDeviceManager& deviceManager) : _audioDeviceManager{deviceManager}
{
    setOpaque(true);

    addAndMakeVisible(_resultsBox);
    _resultsBox.setMultiLine(true);
    _resultsBox.setReturnKeyStartsNewLine(true);
    _resultsBox.setReadOnly(true);
    _resultsBox.setScrollbarsShown(true);
    _resultsBox.setCaretVisible(false);
    _resultsBox.setPopupMenuEnabled(true);

    _resultsBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    _resultsBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    _resultsBox.setText("Running this test measures the round-trip latency between the audio output and input "
                        "devices you\'ve got selected.\n\n"
                        "It\'ll play a sound, then try to measure the time at which the sound arrives "
                        "back at the audio input. Obviously for this to work you need to have your "
                        "microphone somewhere near your speakers...");

    addAndMakeVisible(_startTestButton);
    _startTestButton.onClick = [this] { startTest(); };

    setSize(500, 500);
}

LatencyTesterEditor::~LatencyTesterEditor()
{
    _audioDeviceManager.removeAudioCallback(_latencyTester.get());
    _latencyTester.reset();
}

void LatencyTesterEditor::startTest()
{
    if (_latencyTester == nullptr) {
        _latencyTester = std::make_unique<LatencyTester>(_resultsBox);
        _audioDeviceManager.addAudioCallback(_latencyTester.get());
    }

    _latencyTester->beginTest();
}

void LatencyTesterEditor::paint(juce::Graphics& g) { g.fillAll(findColour(juce::ResizableWindow::backgroundColourId)); }

void LatencyTesterEditor::resized()
{
    auto b = getLocalBounds().reduced(5);
    _startTestButton.setBounds(b.removeFromBottom(b.getHeight() / 10));
    b.removeFromBottom(10);
    _resultsBox.setBounds(b);
}

}  // namespace ra
