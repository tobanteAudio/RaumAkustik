#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {

struct SettingsWindow final
    : juce::PreferencesPanel
    , juce::ChangeListener
{
    explicit SettingsWindow(juce::ApplicationCommandManager& commandManager);
    ~SettingsWindow() override;

    auto createComponentForPage(juce::String const& pageName) -> juce::Component* override;
    auto changeListenerCallback(juce::ChangeBroadcaster* source) -> void override;

private:
    juce::ApplicationCommandManager& _commandManager;
    juce::ValueTree _tree{"Settings"};
};

}  // namespace ra
