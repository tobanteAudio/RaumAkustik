#pragma once

#include <mc/raum_akustik/widget/scrolling_waveform.hpp>

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace mc
{

class AudioRecorder : public juce::AudioIODeviceCallback
{
public:
    AudioRecorder(juce::AudioThumbnail& thumbnailToUpdate) : thumbnail(thumbnailToUpdate)
    {
        backgroundThread.startThread();
    }

    ~AudioRecorder() override { stop(); }

    void startRecording(juce::File const& file)
    {
        stop();

        if (sampleRate > 0)
        {
            // Create an OutputStream to write to our destination file...
            file.deleteFile();

            if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream()))
            {
                // Now create a WAV writer object that writes to our output stream...
                juce::WavAudioFormat wavFormat;

                if (auto writer = wavFormat.createWriterFor(fileStream.get(), sampleRate, 1, 16, {}, 0))
                {
                    fileStream.release();  // (passes responsibility for deleting the stream to the writer object that
                                           // is now using it)

                    // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                    // write the data to disk on our background thread.
                    threadedWriter.reset(new juce::AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));

                    // Reset our recording thumbnail
                    thumbnail.reset(writer->getNumChannels(), writer->getSampleRate());
                    nextSampleNum = 0;

                    // And now, swap over our active writer pointer so that the audio callback will start using it..
                    const juce::ScopedLock sl(writerLock);
                    activeWriter = threadedWriter.get();
                }
            }
        }
    }

    void stop()
    {
        // First, clear this pointer to stop the audio callback from using our writer object..
        {
            const juce::ScopedLock sl(writerLock);
            activeWriter = nullptr;
        }

        // Now we can delete the writer object. It's done in this order because the deletion could
        // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
        // the audio callback while this happens.
        threadedWriter.reset();
    }

    bool isRecording() const { return activeWriter.load() != nullptr; }

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override { sampleRate = device->getCurrentSampleRate(); }

    void audioDeviceStopped() override { sampleRate = 0; }

    void audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels, int numSamples,
                                          juce::AudioIODeviceCallbackContext const& context) override
    {
        ignoreUnused(context);

        const juce::ScopedLock sl(writerLock);

        if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels())
        {
            activeWriter.load()->write(inputChannelData, numSamples);

            // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply
            // references our input data
            juce::AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), thumbnail.getNumChannels(),
                                            numSamples);
            thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);
            nextSampleNum += numSamples;
        }

        // We need to clear the output buffers, in case they're full of junk..
        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != nullptr) juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
    }

private:
    juce::AudioThumbnail& thumbnail;
    juce::TimeSliceThread backgroundThread{
        "Audio Recorder Thread"};  // the thread that will write our audio data to disk
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter>
        threadedWriter;  // the FIFO used to buffer the incoming data
    double sampleRate         = 0.0;
    juce::int64 nextSampleNum = 0;

    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter{nullptr};
};

class RecordingThumbnail
    : public juce::Component
    , private juce::ChangeListener
{
public:
    RecordingThumbnail()
    {
        formatManager.registerBasicFormats();
        thumbnail.addChangeListener(this);
    }

    ~RecordingThumbnail() override { thumbnail.removeChangeListener(this); }

    juce::AudioThumbnail& getAudioThumbnail() { return thumbnail; }

    void setDisplayFullThumbnail(bool displayFull)
    {
        displayFullThumb = displayFull;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::lightgrey);

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto endTime = displayFullThumb ? thumbnail.getTotalLength() : juce::jmax(30.0, thumbnail.getTotalLength());

            auto thumbArea = getLocalBounds();
            thumbnail.drawChannels(g, thumbArea.reduced(2), 0.0, endTime, 1.0f);
        }
        else
        {
            g.setFont(14.0f);
            g.drawFittedText("(No file recorded)", getLocalBounds(), juce::Justification::centred, 2);
        }
    }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{10};
    juce::AudioThumbnail thumbnail{512, formatManager, thumbnailCache};

    bool displayFullThumb = false;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail) repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingThumbnail)
};

inline juce::Colour getUIColourIfAvailable(juce::LookAndFeel_V4::ColourScheme::UIColour uiColour,
                                           juce::Colour fallback = juce::Colour(0xff4d4d4d)) noexcept
{
    if (auto* v4 = dynamic_cast<juce::LookAndFeel_V4*>(&juce::LookAndFeel::getDefaultLookAndFeel()))
        return v4->getCurrentColourScheme().getUIColour(uiColour);

    return fallback;
}

class MeasurementRecorder : public juce::Component
{
public:
    MeasurementRecorder(juce::AudioDeviceManager& audioDeviceManager) : audioDeviceManager{audioDeviceManager}
    {
        setOpaque(true);
        addAndMakeVisible(liveAudioScroller);

        addAndMakeVisible(explanationLabel);
        explanationLabel.setFont(juce::Font(15.0f, juce::Font::plain));
        explanationLabel.setJustificationType(juce::Justification::topLeft);
        explanationLabel.setEditable(false, false, false);
        explanationLabel.setColour(juce::TextEditor::textColourId, juce::Colours::black);
        explanationLabel.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

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

        addAndMakeVisible(recordingThumbnail);

        audioDeviceManager.addAudioCallback(&liveAudioScroller);
        audioDeviceManager.addAudioCallback(&recorder);

        setSize(500, 500);
    }

    ~MeasurementRecorder() override
    {
        audioDeviceManager.removeAudioCallback(&recorder);
        audioDeviceManager.removeAudioCallback(&liveAudioScroller);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getUIColourIfAvailable(juce::LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
    }

    void resized() override
    {
        auto area = getLocalBounds();

        liveAudioScroller.setBounds(area.removeFromTop(80).reduced(8));
        recordingThumbnail.setBounds(area.removeFromTop(80).reduced(8));
        recordButton.setBounds(area.removeFromTop(36).removeFromLeft(140).reduced(8));
        explanationLabel.setBounds(area.reduced(8));
    }

private:
    juce::AudioDeviceManager& audioDeviceManager;

    ScrollingWaveform liveAudioScroller;
    RecordingThumbnail recordingThumbnail;
    AudioRecorder recorder{recordingThumbnail.getAudioThumbnail()};

    juce::Label explanationLabel{{},
                                 "This page demonstrates how to record a wave file from the live audio input..\n\n"
                                 "Pressing record will start recording a file in your \"Documents\" folder."};
    juce::TextButton recordButton{"Record"};
    juce::File lastRecording;

    void startRecording()
    {

        auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
        lastRecording  = parentDir.getNonexistentChildFile("JUCE Demo Audio Recording", ".wav");

        recorder.startRecording(lastRecording);

        recordButton.setButtonText("Stop");
        recordingThumbnail.setDisplayFullThumbnail(false);
    }

    void stopRecording()
    {
        recorder.stop();

        lastRecording = juce::File();
        recordButton.setButtonText("Record");
        recordingThumbnail.setDisplayFullThumbnail(true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeasurementRecorder)
};

}  // namespace mc
