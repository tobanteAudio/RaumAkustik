#include "measurement_recorder.hpp"

#include <memory>

namespace ra {

MeasurementRecorder::MeasurementRecorder(juce::AudioThumbnail& thumbnail) : _thumbnail(thumbnail)
{
    _writerThread.startThread();
}

MeasurementRecorder::~MeasurementRecorder() { stop(); }

void MeasurementRecorder::startRecording(juce::File const& file)
{
    stop();

    if (_sampleRate > 0) {
        // Create an OutputStream to write to our destination file...
        file.deleteFile();

        if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream())) {
            // Now create a WAV writer object that writes to our output stream...
            juce::WavAudioFormat wavFormat;

            if (auto* writer = wavFormat.createWriterFor(fileStream.get(), _sampleRate, 1, 16, {}, 0)) {
                fileStream.release();  // (passes responsibility for deleting the stream to the writer object that
                                       // is now using it)

                // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                // write the data to disk on our background thread.
                _writer = std::make_unique<juce::AudioFormatWriter::ThreadedWriter>(writer, _writerThread, 32768);

                // Reset our recording thumbnail
                _thumbnail.reset(writer->getNumChannels(), writer->getSampleRate());
                _nextSampleNum = 0;

                // And now, swap over our active writer pointer so that the audio callback will start using it..
                juce::ScopedLock const sl(_writerLock);
                _activeWriter = _writer.get();
            }
        }
    }

    _doneRecording.store(false);
    startTimer(500);
}

void MeasurementRecorder::timerCallback()
{
    if (not _doneRecording.load()) {
        return;
    }

    stopTimer();
    stop();
}

void MeasurementRecorder::stop()
{
    // First, clear this pointer to stop the audio callback from using our writer object..
    {
        juce::ScopedLock const sl(_writerLock);
        _activeWriter = nullptr;
    }

    // Now we can delete the writer object. It's done in this order because the deletion could
    // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
    // the audio callback while this happens.
    _writer.reset();
}

auto MeasurementRecorder::isRecording() const -> bool { return _activeWriter.load() != nullptr; }

void MeasurementRecorder::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    _sampleRate = device->getCurrentSampleRate();

    _sweep = generate(SineSweep{
        .from       = 20.0 * si::hertz,
        .to         = 20'000.0 * si::hertz,
        .curve      = SineSweepCurve::Logarithmic,
        .duration   = std::chrono::milliseconds{10'000},
        .sampleRate = _sampleRate,
    });
}

void MeasurementRecorder::audioDeviceStopped() { _sampleRate = 0; }

void MeasurementRecorder::audioDeviceIOCallbackWithContext(
    float const* const* inputChannelData,
    int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    juce::AudioIODeviceCallbackContext const& context
)
{
    ignoreUnused(context);

    // We need to clear the output buffers, in case they're full of junk..
    for (int i = 0; i < numOutputChannels; ++i) {
        if (outputChannelData[i] != nullptr) {
            juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
        }
    }

    juce::ScopedLock const sl(_writerLock);

    auto const lastSample = std::ssize(_sweep) + juce::roundToInt(_sampleRate * 2.0);

    if (_activeWriter.load() != nullptr && numInputChannels >= _thumbnail.getNumChannels()) {
        _activeWriter.load()->write(inputChannelData, numSamples);

        // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply
        // references our input data
        juce::AudioBuffer<float> const buffer(
            const_cast<float**>(inputChannelData),
            _thumbnail.getNumChannels(),
            numSamples
        );
        _thumbnail.addBlock(_nextSampleNum, buffer, 0, numSamples);

        if (numOutputChannels > 0) {
            auto const* sweep = std::data(_sweep);
            auto const* end   = std::next(sweep, std::ssize(_sweep));
            auto const* first = std::min(end, std::next(sweep, _nextSampleNum));
            auto const* last  = std::min(end, std::next(first, numSamples));
            std::copy(first, last, outputChannelData[0]);
        }

        _nextSampleNum += numSamples;
    }

    if (_nextSampleNum >= lastSample) {
        _doneRecording.store(true);
    }
}

MeasurementRecorderEditor::MeasurementRecorderEditor(juce::AudioDeviceManager& deviceManager)
    : _audioDeviceManager{deviceManager}
{
    setOpaque(true);

    addAndMakeVisible(_recordButton);
    _recordButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff5c5c));
    _recordButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    _recordButton.onClick = [this] {
        if (_recorder.isRecording()) {
            stopRecording();
        } else {
            startRecording();
        }
    };

    addAndMakeVisible(_thumbnail);

    _audioDeviceManager.addAudioCallback(&_recorder);

    setSize(500, 500);
}

MeasurementRecorderEditor::~MeasurementRecorderEditor() { _audioDeviceManager.removeAudioCallback(&_recorder); }

void MeasurementRecorderEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MeasurementRecorderEditor::resized()
{
    auto area = getLocalBounds();

    _thumbnail.setBounds(area.removeFromTop(80).reduced(8));
    _recordButton.setBounds(area.removeFromTop(36).removeFromLeft(140).reduced(8));
}

MeasurementRecorderEditor::Thumbnail::Thumbnail()
{
    _formatManager.registerBasicFormats();
    _thumbnail.addChangeListener(this);
}

MeasurementRecorderEditor::Thumbnail::~Thumbnail() { _thumbnail.removeChangeListener(this); }

auto MeasurementRecorderEditor::Thumbnail::getAudioThumbnail() -> juce::AudioThumbnail& { return _thumbnail; }

void MeasurementRecorderEditor::Thumbnail::setDisplayFullThumbnail(bool displayFull)
{
    _displayFullThumb = displayFull;
    repaint();
}

void MeasurementRecorderEditor::Thumbnail::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);

    if (_thumbnail.getTotalLength() > 0.0) {
        auto endTime = _displayFullThumb ? _thumbnail.getTotalLength() : juce::jmax(30.0, _thumbnail.getTotalLength());

        auto thumbArea = getLocalBounds();
        _thumbnail.drawChannels(g, thumbArea.reduced(2), 0.0, endTime, 1.0F);
    } else {
        g.setFont(14.0F);
        g.drawFittedText("(No file recorded)", getLocalBounds(), juce::Justification::centred, 2);
    }
}

void MeasurementRecorderEditor::Thumbnail::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &_thumbnail) {
        repaint();
    }
}

void MeasurementRecorderEditor::startRecording()
{

    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    _lastRecording = parentDir.getNonexistentChildFile("JUCE Demo Audio Recording", ".wav");

    _recorder.startRecording(_lastRecording);

    _recordButton.setButtonText("Stop");
    _thumbnail.setDisplayFullThumbnail(false);
}

void MeasurementRecorderEditor::stopRecording()
{
    _recorder.stop();

    _lastRecording = juce::File();
    _recordButton.setButtonText("Record");
    _thumbnail.setDisplayFullThumbnail(true);
}

}  // namespace ra
