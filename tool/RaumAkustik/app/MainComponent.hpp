#pragma once

#include "app/MenuBar.hpp"
#include "look/LookAndFeel.hpp"
#include "tabs/AudioInterfaceEditor.hpp"
#include "tabs/MaterialEditor.hpp"
#include "tabs/PorousAbsorberEditor.hpp"
#include "tabs/RoomEditor.hpp"
#include "tabs/StochasticRaytracingEditor.hpp"
#include "tabs/ToneGeneratorEditor.hpp"
#include "tabs/WaveEquation2DEditor.hpp"
#include "widget/LevelMeter.hpp"
#include "widget/ScrollingWaveform.hpp"

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

    juce::ThreadPool _threadPool;
    juce::ApplicationCommandManager _commandManager;
    juce::UndoManager _undoManager;
    juce::ValueTree _valueTree{"RaumAkustik"};
    LookAndFeel _lnf;

    MenuBar _menuBar{_commandManager};
    LevelMeter _levelMeter;
    ScrollingWaveform _waveform;
    juce::TabbedComponent _tabs{juce::TabbedButtonBar::TabsAtTop};

    std::unique_ptr<RoomEditor> _roomEditor;
    AudioInterfaceEditor _audioInputEditor;
    ToneGeneratorEditor _generatorEditor;
    std::unique_ptr<StochasticRaytracingEditor> _raytracingEditor;
    std::unique_ptr<WaveEquation2DEditor> _waveEquationEditor;
    std::unique_ptr<PorousAbsorberEditor> _absorberSimulationEditor;
    std::unique_ptr<MaterialEditor> _materialEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)  // NOLINT
};

}  // namespace ra
