#pragma once

#include <mc/generator/sweep.hpp>
#include <mc/raum_akustik/widget/scrolling_waveform.hpp>

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ra
{

struct MeasurementRecorder final
    : juce::Timer
    , juce::AudioIODeviceCallback
{
    explicit MeasurementRecorder(juce::AudioThumbnail& thumbnail);
    ~MeasurementRecorder() override;

    void startRecording(juce::File const& file);
    void stop();
    bool isRecording() const;

    void timerCallback() override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallbackWithContext(float const* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels, int numSamples,
                                          juce::AudioIODeviceCallbackContext const& context) override;

private:
    juce::AudioThumbnail& _thumbnail;
    juce::TimeSliceThread _writerThread{"Audio Recorder Thread"};
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> _writer;
    double _sampleRate{0.0};
    juce::int64 nextSampleNum{0};

    std::vector<float> _sweep{};
    std::atomic<bool> _doneRecording{false};

    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter{nullptr};
};

struct MeasurementRecorderEditor final : juce::Component
{
    explicit MeasurementRecorderEditor(juce::AudioDeviceManager& audioDeviceManager);
    ~MeasurementRecorderEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    struct Thumbnail
        : juce::Component
        , juce::ChangeListener
    {
        Thumbnail();
        ~Thumbnail() override;

        juce::AudioThumbnail& getAudioThumbnail();
        void setDisplayFullThumbnail(bool displayFull);

        void paint(juce::Graphics& g) override;
        void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    private:
        juce::AudioFormatManager formatManager;
        juce::AudioThumbnailCache thumbnailCache{10};
        juce::AudioThumbnail _thumbnail{512, formatManager, thumbnailCache};

        bool displayFullThumb{false};
    };

    void startRecording();
    void stopRecording();

    juce::AudioDeviceManager& audioDeviceManager;

    Thumbnail _thumbnail;
    MeasurementRecorder recorder{_thumbnail.getAudioThumbnail()};

    juce::TextButton recordButton{"Record"};
    juce::File lastRecording;
};

}  // namespace ra
