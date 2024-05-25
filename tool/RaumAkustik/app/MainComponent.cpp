#include "MainComponent.hpp"

#include "app/CommandIDs.hpp"
#include "app/application.hpp"
#include "tool/AudioFile.hpp"
#include "utility/ValueTree.hpp"

namespace ra {

MainComponent::MainComponent()
    : _audioInputEditor{raumAkusticApplication().deviceManager()}
    , _generatorEditor{raumAkusticApplication().deviceManager()}
{
    auto room = juce::ValueTree{"Room"};
    setPropertyIfNotExist(room, "icon_size", 50.0);

    setPropertyIfNotExist(room, "length", 6.0);
    setPropertyIfNotExist(room, "width", 3.65);
    setPropertyIfNotExist(room, "height", 3.12);

    setPropertyIfNotExist(room, "listen_x", 1.83);
    setPropertyIfNotExist(room, "listen_y", 2.00);
    setPropertyIfNotExist(room, "listen_z", 1.00);

    setPropertyIfNotExist(room, "left_x", 1.33);
    setPropertyIfNotExist(room, "left_y", 1.00);
    setPropertyIfNotExist(room, "left_z", 1.20);

    setPropertyIfNotExist(room, "right_x", 2.33);
    setPropertyIfNotExist(room, "right_y", 1.00);
    setPropertyIfNotExist(room, "right_z", 1.20);

    _valueTree.appendChild(room, nullptr);

    _projectTree.setRootItem(&_treeViewRoot);

    addAndMakeVisible(_menuBar);
    addAndMakeVisible(_projectTree);
    addAndMakeVisible(_levelMeter);
    addAndMakeVisible(_waveform);
    addAndMakeVisible(_tabs);

    _commandManager.registerAllCommandsForTarget(this);
    addKeyListener(_commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);

    juce::LookAndFeel::setDefaultLookAndFeel(&_lnf);
    setLookAndFeel(&_lnf);
    setSize(1280, 720);

    raumAkusticApplication().deviceManager().addAudioCallback(&_levelMeter);
    raumAkusticApplication().deviceManager().addAudioCallback(&_waveform);

    reloadUI();
}

MainComponent::~MainComponent()
{
    DBG(_valueTree.toXmlString());
    raumAkusticApplication().deviceManager().removeAudioCallback(&_levelMeter);
    raumAkusticApplication().deviceManager().removeAudioCallback(&_waveform);
    _threadPool.removeAllJobs(true, 500, nullptr);
    setLookAndFeel(nullptr);
}

auto MainComponent::paint(juce::Graphics& g) -> void
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

auto MainComponent::resized() -> void
{
    auto area = getLocalBounds();

    _menuBar.setBounds(area.removeFromTop(juce::LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));

    if (_waveform.isVisible()) {
        _waveform.setBounds(area.removeFromTop(area.proportionOfHeight(0.075)));
    }
    if (_levelMeter.isVisible()) {
        _levelMeter.setBounds(area.removeFromRight(area.proportionOfWidth(0.075)));
    }

    _projectTree.setBounds(area.removeFromLeft(area.proportionOfWidth(0.125)));
    _tabs.setBounds(area.reduced(10));
}

auto MainComponent::getNextCommandTarget() -> juce::ApplicationCommandTarget*
{
    return findFirstTargetParentComponent();
}

auto MainComponent::getAllCommands(juce::Array<juce::CommandID>& c) -> void
{
    c.addArray({
        CommandIDs::open,
        CommandIDs::save,
        CommandIDs::saveAs,
        CommandIDs::redo,
        CommandIDs::undo,
        CommandIDs::settings,
        CommandIDs::gotoTabLeft,
        CommandIDs::gotoTabRight,
        CommandIDs::fullscreen,
        CommandIDs::toggleLevelMeter,
        CommandIDs::toggleWaveform,
        CommandIDs::about,
    });
}

auto MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) -> void
{
    using juce::KeyPress;
    using juce::ModifierKeys;

    switch (commandID) {
        case CommandIDs::open:
            result.setInfo("Open", "Opens a project file", "File", 0);
            result.addDefaultKeypress('o', ModifierKeys::commandModifier);
            break;
        case CommandIDs::save:
            result.setInfo("Save", "Saves a project file", "File", 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            break;
        case CommandIDs::saveAs:
            result.setInfo("Save As", "Saves a project file to a new location", "File", 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            break;
        case CommandIDs::undo:
            result.setInfo("Undo", "Undo one step", "Edit", 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier);
            break;
        case CommandIDs::redo:
            result.setInfo("Redo", "Redo one step", "Edit", 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            break;
        case CommandIDs::settings:
            result.setInfo("Settings", "Open settings", "Edit", 0);
            result.addDefaultKeypress('.', ModifierKeys::commandModifier);
            break;
        case CommandIDs::gotoTabLeft:
            result.setInfo("Goto tab left", "Goto tab on the left", "View", 0);
            result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::commandModifier);
            break;
        case CommandIDs::gotoTabRight:
            result.setInfo("Goto tab right", "Goto tab on the right", "View", 0);
            result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            break;
        case CommandIDs::fullscreen:
            result.setInfo("Fullscreen", "Toggle fullscreen mode", "View", 0);
            result.setTicked(getPeer() == nullptr ? false : getPeer()->isFullScreen());
            result.addDefaultKeypress(KeyPress::F11Key, ModifierKeys::noModifiers);
            break;
        case CommandIDs::toggleLevelMeter:
            result.setInfo("Toggle Level Meter", "Toggle level-meter", "View", 0);
            result.setTicked(_levelMeter.isVisible());
            break;
        case CommandIDs::toggleWaveform:
            result.setInfo("Toggle Waveform", "Toggle scrolling waveform", "View", 0);
            result.setTicked(_waveform.isVisible());
            break;
        case CommandIDs::about:
            result.setInfo("About", "Open about page", "Help", 0);
            result.addDefaultKeypress('h', ModifierKeys::commandModifier);
            break;
        default: break;
    }
}

auto MainComponent::perform(juce::ApplicationCommandTarget::InvocationInfo const& info) -> bool
{
    switch (info.commandID) {
        case CommandIDs::open: loadProject(); break;
        case CommandIDs::save:
        case CommandIDs::saveAs: saveProject(); break;
        case CommandIDs::undo: _undoManager.undo(); break;
        case CommandIDs::redo: _undoManager.redo(); break;
        case CommandIDs::fullscreen: toggleFullscreen(); break;
        case CommandIDs::toggleLevelMeter:
            _levelMeter.setVisible(not _levelMeter.isVisible());
            resized();
            break;
        case CommandIDs::toggleWaveform:
            _waveform.setVisible(not _waveform.isVisible());
            resized();
            break;
        case CommandIDs::about: showAboutMessage(); break;
        default: return false;
    }

    return true;
}

auto MainComponent::showAboutMessage() -> void
{
    juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon, "About", "Version 0.1.0", this);
}

auto MainComponent::saveProject() -> void
{
    auto file = juce::File{juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    if (file.existsAsFile()) {
        file.deleteFile();
    }
    auto stream = juce::FileOutputStream{file};
    stream.writeText(_valueTree.toXmlString(), false, false, "\n");
}

auto MainComponent::loadProject() -> void
{
    auto file   = juce::File{juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    auto stream = juce::FileInputStream{file};
    _valueTree  = juce::ValueTree::fromXml(stream.readEntireStreamAsString());
    reloadUI();
}

auto MainComponent::reloadUI() -> void
{
    auto tabIndex = _tabs.getCurrentTabIndex();
    _tabs.clearTabs();

    _materialEditor.reset();
    _absorberSimulationEditor.reset();
    _raytracingEditor.reset();
    _waveEquationEditor.reset();
    _roomEditor.reset();

    auto absorberTree = PorousAbsorberEditor::createDefault(_valueTree, &_undoManager);

    _roomEditor               = std::make_unique<RoomEditor>(_valueTree, &_undoManager);
    _raytracingEditor         = std::make_unique<StochasticRaytracingEditor>(_threadPool, *_roomEditor);
    _waveEquationEditor       = std::make_unique<WaveEquation2DEditor>(_threadPool, *_roomEditor);
    _absorberSimulationEditor = std::make_unique<PorousAbsorberEditor>(absorberTree, &_undoManager);
    _materialEditor           = std::make_unique<MaterialEditor>();

    auto const color = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    _tabs.addTab("Room", color, _roomEditor.get(), false);
    _tabs.addTab("Audio Input", color, std::addressof(_audioInputEditor), false);
    _tabs.addTab("Generator", color, std::addressof(_generatorEditor), false);
    _tabs.addTab("Raytracing", color, _raytracingEditor.get(), false);
    _tabs.addTab("Wave Equation 2D", color, _waveEquationEditor.get(), false);
    _tabs.addTab("Porous Absorber", color, _absorberSimulationEditor.get(), false);
    _tabs.addTab("Material", color, _materialEditor.get(), false);
    if (tabIndex > 0 and tabIndex <= _tabs.getNumTabs()) {
        _tabs.setCurrentTabIndex(tabIndex, true);
    }
}

auto MainComponent::toggleFullscreen() -> void { getPeer()->setFullScreen(!getPeer()->isFullScreen()); }

}  // namespace ra
