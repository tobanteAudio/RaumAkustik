#pragma once

#include "tool/MeasurementRecorder.hpp"

#include <neo/fft.hpp>

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {

struct ToneGeneratorEditor final
    : juce::Component
    , juce::AsyncUpdater
    , juce::ChangeListener
    , juce::ValueTree::Listener
{
    explicit ToneGeneratorEditor(juce::AudioDeviceManager& audioDeviceManager);
    ~ToneGeneratorEditor() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;
    auto handleAsyncUpdate() -> void override;
    auto changeListenerCallback(juce::ChangeBroadcaster* source) -> void override;
    auto valueTreePropertyChanged(juce::ValueTree& tree, juce::Identifier const& property) -> void override;

private:
    juce::PropertyPanel _sweepSpecPanel;
    MeasurementRecorderEditor _recorder;
    juce::Rectangle<int> _thumbnailBounds;
    juce::Rectangle<int> _spectrumBounds;

    juce::UndoManager* _undoManager{nullptr};
    juce::ValueTree _valueTree{"GlideSweep"};

    juce::CachedValue<double> _from{_valueTree, "from", _undoManager};
    juce::CachedValue<double> _to{_valueTree, "to", _undoManager};
    juce::CachedValue<bool> _curve{_valueTree, "curve", _undoManager};
    juce::CachedValue<double> _duration{_valueTree, "duration", _undoManager};
    juce::CachedValue<double> _sampleRate{_valueTree, "sampleRate", _undoManager};

    juce::AudioFormatManager _formatManager;
    juce::AudioBuffer<float> _thumbnailBuffer;
    juce::AudioThumbnailCache _thumbnailCache{1};
    juce::AudioThumbnail _thumbnail{32, _formatManager, _thumbnailCache};

    juce::Path _spectrumPath;
    std::unique_ptr<neo::fft::rfft_plan<float>> _fft;
};

}  // namespace ra
