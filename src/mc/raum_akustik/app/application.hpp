#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace mc
{
struct RaumAkustikApplication : juce::JUCEApplication
{
    RaumAkustikApplication() = default;

    auto getApplicationName() -> const juce::String override;
    auto getApplicationVersion() -> const juce::String override;
    auto moreThanOneInstanceAllowed() -> bool override;

    auto initialise(juce::String const& commandLine) -> void override;
    auto shutdown() -> void override;
    auto systemRequestedQuit() -> void override;
    auto anotherInstanceStarted(juce::String const& commandLine) -> void override;

    auto deviceManager() -> juce::AudioDeviceManager& { return _deviceManager; }

private:
    juce::AudioDeviceManager _deviceManager;
    std::unique_ptr<juce::DocumentWindow> _mainWindow{};
};

[[nodiscard]] auto raumAkusticApplication() -> RaumAkustikApplication&;
}  // namespace mc