#pragma once

#include "PorousAbsorberSimulationView.hpp"
#include "RoomSimulationView.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

struct MainComponent final : juce::Component
{
    MainComponent();
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::TabbedComponent _tabs {juce::TabbedButtonBar::TabsAtTop};

    mc::RoomSimulationView _roomSimulationView;
    mc::PorousAbsorberSimulationView _absorberSimulationView;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
