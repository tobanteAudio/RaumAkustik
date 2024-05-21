#pragma once

#include "app/menu_bar.hpp"
#include "look/look_and_feel.hpp"
#include "tabs/absorber_simulation_tab.hpp"
#include "tabs/audio_input_tab.hpp"
#include "tabs/generator_tab.hpp"
#include "tabs/material_editor.hpp"
#include "tabs/room_editor.hpp"
#include "tabs/stochastic_raytracing_tab.hpp"
#include "widget/level_meter.hpp"
#include "widget/scrolling_waveform.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {
struct MainComponent final
    : juce::Component
    , juce::ApplicationCommandTarget
{
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
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

    juce::ApplicationCommandManager _commandManager;
    juce::UndoManager _undoManager;
    juce::ValueTree _valueTree{"RaumAkustik"};
    LookAndFeel _lnf;

    MenuBar _menuBar{_commandManager};
    LevelMeter _levelMeter;
    ScrollingWaveform _waveform;
    juce::TabbedComponent _tabs{juce::TabbedButtonBar::TabsAtTop};

    std::unique_ptr<RoomEditor> _roomEditor;
    AudioInputEditor _audioInputEditor;
    GeberatorEditor _generatorEditor;
    std::unique_ptr<StochasticRaytracingEditor> _raytracingEditor;
    std::unique_ptr<PorousAbsorberSimulationEditor> _absorberSimulationEditor;
    std::unique_ptr<MaterialEditor> _materialEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)  // NOLINT
};

}  // namespace ra