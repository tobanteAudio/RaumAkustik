#pragma once

#include <mc/raum_akustik/app/main_component.hpp>
#include <mc/raum_akustik/app/menu_bar.hpp>
#include <mc/raum_akustik/look/look_and_feel.hpp>
#include <mc/raum_akustik/tabs/absorber_simulation_tab.hpp>
#include <mc/raum_akustik/tabs/audio_input_tab.hpp>
#include <mc/raum_akustik/tabs/first_reflections_tab.hpp>
#include <mc/raum_akustik/tabs/generator_tab.hpp>
#include <mc/raum_akustik/widget/level_meter.hpp>
#include <mc/raum_akustik/widget/scrolling_waveform.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

namespace mc
{
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

    juce::ApplicationCommandManager _commandManager{};
    juce::UndoManager _undoManager;
    juce::ValueTree _valueTree{"RaumAkustik"};
    LookAndFeel _lnf{};

    MenuBar _menuBar{_commandManager};
    LevelMeter _levelMeter;
    ScrollingWaveform _waveform;

    juce::TabbedComponent _tabs{juce::TabbedButtonBar::TabsAtTop};
    AudioInputView _audioInputView;
    GeneratorTab _generatorTab;
    std::unique_ptr<FirstReflectionsView> _firstReflectionsView{};
    std::unique_ptr<PorousAbsorberSimulationView> _absorberSimulationView{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)  // NOLINT
};

}  // namespace mc
