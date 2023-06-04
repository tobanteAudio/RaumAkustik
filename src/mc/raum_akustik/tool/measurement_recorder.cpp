#include "measurement_recorder.hpp"

namespace mc
{

MeasurementRecorder::MeasurementRecorder(juce::AudioThumbnail& thumbnail) : _thumbnail(thumbnail)
{
    _writerThread.startThread();
}
MeasurementRecorder::~MeasurementRecorder() { stop(); }

void MeasurementRecorder::startRecording(juce::File const& file)
{
    stop();

    if (_sampleRate > 0)
    {
        // Create an OutputStream to write to our destination file...
        file.deleteFile();

        if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream()))
        {
            // Now create a WAV writer object that writes to our output stream...
            juce::WavAudioFormat wavFormat;

            if (auto writer = wavFormat.createWriterFor(fileStream.get(), _sampleRate, 1, 16, {}, 0))
            {
                fileStream.release();  // (passes responsibility for deleting the stream to the writer object that
                                       // is now using it)

                // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                // write the data to disk on our background thread.
                _writer.reset(new juce::AudioFormatWriter::ThreadedWriter(writer, _writerThread, 32768));

                // Reset our recording thumbnail
                _thumbnail.reset(writer->getNumChannels(), writer->getSampleRate());
                nextSampleNum = 0;

                // And now, swap over our active writer pointer so that the audio callback will start using it..
                const juce::ScopedLock sl(writerLock);
                activeWriter = _writer.get();
            }
        }
    }

    _doneRecording.store(false);
    startTimer(500);
}

void MeasurementRecorder::timerCallback()
{
    if (not _doneRecording.load()) { return; }

    stopTimer();
    stop();
}

void MeasurementRecorder::stop()
{
    // First, clear this pointer to stop the audio callback from using our writer object..
    {
        const juce::ScopedLock sl(writerLock);
        activeWriter = nullptr;
    }

    // Now we can delete the writer object. It's done in this order because the deletion could
    // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
    // the audio callback while this happens.
    _writer.reset();
}

bool MeasurementRecorder::isRecording() const { return activeWriter.load() != nullptr; }

void MeasurementRecorder::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    _sampleRate = device->getCurrentSampleRate();

    _sweep = generate(SineSweep{
        .from       = Hertz<double>{20.0},
        .to         = Hertz<double>{20'000.0},
        .curve      = SineSweepCurve::Logarithmic,
        .duration   = std::chrono::milliseconds{10'000},
        .sampleRate = _sampleRate,
    });
}

void MeasurementRecorder::audioDeviceStopped() { _sampleRate = 0; }

void MeasurementRecorder::audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                                           float* const* outputChannelData, int numOutputChannels,
                                                           int numSamples,
                                                           juce::AudioIODeviceCallbackContext const& context)
{
    ignoreUnused(context);

    // We need to clear the output buffers, in case they're full of junk..
    for (int i = 0; i < numOutputChannels; ++i)
    {
        if (outputChannelData[i] != nullptr) { juce::FloatVectorOperations::clear(outputChannelData[i], numSamples); }
    }

    const juce::ScopedLock sl(writerLock);

    auto const lastSample = std::ssize(_sweep) + juce::roundToInt(_sampleRate * 2.0);

    if (activeWriter.load() != nullptr && numInputChannels >= _thumbnail.getNumChannels())
    {
        activeWriter.load()->write(inputChannelData, numSamples);

        // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply
        // references our input data
        juce::AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), _thumbnail.getNumChannels(), numSamples);
        _thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);

        if (numOutputChannels > 0)
        {
            auto const* sweep = std::data(_sweep);
            auto const* end   = std::next(sweep, std::ssize(_sweep));
            auto const* first = std::min(end, std::next(sweep, nextSampleNum));
            auto const* last  = std::min(end, std::next(first, numSamples));
            std::copy(first, last, outputChannelData[0]);
        }

        nextSampleNum += numSamples;
    }

    if (nextSampleNum >= lastSample) { _doneRecording.store(true); }
}

MeasurementRecorderEditor::MeasurementRecorderEditor(juce::AudioDeviceManager& deviceManager)
    : audioDeviceManager{deviceManager}
{
    setOpaque(true);

    addAndMakeVisible(recordButton);
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff5c5c));
    recordButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    recordButton.onClick = [this]
    {
        if (recorder.isRecording())
            stopRecording();
        else
            startRecording();
    };

    addAndMakeVisible(_thumbnail);

    audioDeviceManager.addAudioCallback(&recorder);

    setSize(500, 500);
}

MeasurementRecorderEditor::~MeasurementRecorderEditor() { audioDeviceManager.removeAudioCallback(&recorder); }

void MeasurementRecorderEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MeasurementRecorderEditor::resized()
{
    auto area = getLocalBounds();

    _thumbnail.setBounds(area.removeFromTop(80).reduced(8));
    recordButton.setBounds(area.removeFromTop(36).removeFromLeft(140).reduced(8));
}

MeasurementRecorderEditor::Thumbnail::Thumbnail()
{
    formatManager.registerBasicFormats();
    _thumbnail.addChangeListener(this);
}

MeasurementRecorderEditor::Thumbnail::~Thumbnail() { _thumbnail.removeChangeListener(this); }

juce::AudioThumbnail& MeasurementRecorderEditor::Thumbnail::getAudioThumbnail() { return _thumbnail; }

void MeasurementRecorderEditor::Thumbnail::setDisplayFullThumbnail(bool displayFull)
{
    displayFullThumb = displayFull;
    repaint();
}

void MeasurementRecorderEditor::Thumbnail::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);

    if (_thumbnail.getTotalLength() > 0.0)
    {
        auto endTime = displayFullThumb ? _thumbnail.getTotalLength() : juce::jmax(30.0, _thumbnail.getTotalLength());

        auto thumbArea = getLocalBounds();
        _thumbnail.drawChannels(g, thumbArea.reduced(2), 0.0, endTime, 1.0f);
    }
    else
    {
        g.setFont(14.0f);
        g.drawFittedText("(No file recorded)", getLocalBounds(), juce::Justification::centred, 2);
    }
}

void MeasurementRecorderEditor::Thumbnail::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &_thumbnail) repaint();
}

void MeasurementRecorderEditor::startRecording()
{

    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    lastRecording  = parentDir.getNonexistentChildFile("JUCE Demo Audio Recording", ".wav");

    recorder.startRecording(lastRecording);

    recordButton.setButtonText("Stop");
    _thumbnail.setDisplayFullThumbnail(false);
}

void MeasurementRecorderEditor::stopRecording()
{
    recorder.stop();

    lastRecording = juce::File();
    recordButton.setButtonText("Record");
    _thumbnail.setDisplayFullThumbnail(true);
}

}  // namespace mc
