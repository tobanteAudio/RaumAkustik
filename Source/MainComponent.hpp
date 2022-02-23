#pragma once

#include "PorousAbsorberSimulationView.hpp"
#include "RoomSimulationView.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

struct MainComponent final : juce::Component
{
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::ValueTree _valueTree {"RaumAkustik"};

    juce::TabbedComponent _tabs {juce::TabbedButtonBar::TabsAtTop};
    mc::RoomSimulationView _roomSimulationView {_valueTree};
    mc::PorousAbsorberSimulationView _absorberSimulationView {_valueTree};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
