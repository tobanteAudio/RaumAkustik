#pragma once

#include "component/Spectogram.hpp"
#include "tool/LatencyTester.hpp"
#include "tool/NoiseGenerator.hpp"

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {

struct AudioInterfaceEditor final : juce::Component
{
    explicit AudioInterfaceEditor(juce::AudioDeviceManager& deviceManager);
    ~AudioInterfaceEditor() override;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

private:
    juce::AudioDeviceManager& _deviceManager;
    juce::AudioDeviceSelectorComponent _deviceSelector;
    LatencyTesterEditor _latencyTester{_deviceManager};
    Spectogram _spectogram;
    NoiseGenerator _noise;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInterfaceEditor)  // NOLINT
};

}  // namespace ra
