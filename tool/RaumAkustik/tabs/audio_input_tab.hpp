#pragma once

#include "tool/latency_tester.hpp"
#include "tool/noise_generator.hpp"
#include "widget/spectogram.hpp"

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {

struct AudioInputEditor final : juce::Component
{
    explicit AudioInputEditor(juce::AudioDeviceManager& deviceManager);
    ~AudioInputEditor() override;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

private:
    juce::AudioDeviceManager& _deviceManager;
    juce::AudioDeviceSelectorComponent _deviceSelector;
    LatencyTesterEditor _latencyTester{_deviceManager};
    Spectogram _spectogram;
    NoiseGenerator _noise;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInputEditor)  // NOLINT
};

}  // namespace ra
