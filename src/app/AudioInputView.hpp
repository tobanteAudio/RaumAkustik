#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace mc
{

struct AudioInputView final : juce::Component
{
    AudioInputView(juce::AudioDeviceManager& deviceManager);
    ~AudioInputView() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

private:
    juce::AudioDeviceManager& _deviceManager;
    juce::AudioDeviceSelectorComponent _deviceSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInputView)  // NOLINT
};

}  // namespace mc
