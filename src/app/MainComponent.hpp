#pragma once

#include "FirstReflectionsView.hpp"
#include "LookAndFeel.hpp"
#include "MenuBar.hpp"
#include "PorousAbsorberSimulationView.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

struct MainComponent final
    : juce::Component
    , juce::ApplicationCommandTarget
{
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    auto getNextCommandTarget() -> juce::ApplicationCommandTarget* override;
    auto getAllCommands(juce::Array<juce::CommandID>& commands) -> void override;
    auto getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) -> void override;
    auto perform(juce::ApplicationCommandTarget::InvocationInfo const& info) -> bool override;

private:
    auto showAboutMessage() -> void;
    auto saveProject() -> void;
    auto loadProject() -> void;
    auto reloadUI() -> void;
    auto toggleFullscreen() -> void;

    juce::ApplicationCommandManager _commandManager{};
    juce::UndoManager _undoManager;
    juce::ValueTree _valueTree{"RaumAkustik"};
    mc::LookAndFeel _lnf{};

    mc::MenuBar _menuBar{_commandManager};

    juce::TabbedComponent _tabs{juce::TabbedButtonBar::TabsAtTop};
    std::unique_ptr<mc::FirstReflectionsView> _firstReflectionsView{};
    std::unique_ptr<mc::PorousAbsorberSimulationView> _absorberSimulationView{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
