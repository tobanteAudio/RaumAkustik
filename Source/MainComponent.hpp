#pragma once

#include "FirstReflectionsView.hpp"
#include "PorousAbsorberSimulationView.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

struct MainComponent final : juce::Component
{
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    auto saveSession() -> void;
    auto loadSession() -> void;
    auto reloadUI() -> void;

    juce::UndoManager _undoManager;
    juce::ValueTree _valueTree {"RaumAkustik"};

    juce::DrawableButton _undo {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton _redo {"Redo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton _load {"Load", juce::DrawableButton::ImageFitted};
    juce::DrawableButton _save {"Save", juce::DrawableButton::ImageFitted};

    juce::TabbedComponent _tabs {juce::TabbedButtonBar::TabsAtTop};
    std::unique_ptr<mc::FirstReflectionsView> _firstReflectionsView {};
    std::unique_ptr<mc::PorousAbsorberSimulationView> _absorberSimulationView {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
