#include "main_component.hpp"

#include <ra/raum_akustik/app/application.hpp>
#include <ra/raum_akustik/app/command_ids.hpp>

namespace ra {
// static auto writeToWavFile(juce::File const& file, std::vector<float> const& buffer, double fs, int bits = 24) ->
// bool
// {
//     if (file.existsAsFile()) { file.deleteFile(); }
//     auto stream = file.createOutputStream();
//     auto wav    = juce::WavAudioFormat{};
//     auto writer = std::unique_ptr<juce::AudioFormatWriter>{wav.createWriterFor(stream.get(), fs, 1, bits, {}, 0)};
//     if (writer) { juce::ignoreUnused(stream.release()); }
//     auto channels = std::array<float const*, 1>{buffer.data()};
//     return writer->writeFromFloatArrays(channels.data(), 1, static_cast<int>(buffer.size()));
// }

MainComponent::MainComponent()
    : _audioInputView{raumAkusticApplication().deviceManager()}
    // , _generatorTab{raumAkusticApplication().deviceManager()}
{

    // writeToWavFile(juce::File{R"(C:\Developer\sweep.wav)"}, sweep, spec.sampleRate, 24);

    addAndMakeVisible(_menuBar);
    addAndMakeVisible(_levelMeter);
    addAndMakeVisible(_waveform);
    addAndMakeVisible(_tabs);

    raumAkusticApplication().deviceManager().addAudioCallback(&_levelMeter);
    raumAkusticApplication().deviceManager().addAudioCallback(&_waveform);

    _commandManager.registerAllCommandsForTarget(this);
    addKeyListener(_commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);

    juce::LookAndFeel::setDefaultLookAndFeel(&_lnf);
    setLookAndFeel(&_lnf);
    setSize(1280, 720);

    reloadUI();
}

MainComponent::~MainComponent()
{
    DBG(_valueTree.toXmlString());
    raumAkusticApplication().deviceManager().removeAudioCallback(&_levelMeter);
    raumAkusticApplication().deviceManager().removeAudioCallback(&_waveform);
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
    _waveform.setBounds(area.removeFromTop(area.proportionOfHeight(0.075)));
    _levelMeter.setBounds(area.removeFromRight(area.proportionOfWidth(0.075)));
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

    _firstReflectionsView   = std::make_unique<FirstReflectionsView>(_valueTree, &_undoManager);
    _absorberSimulationView = std::make_unique<PorousAbsorberSimulationView>(_valueTree, &_undoManager);

    auto const color = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    _tabs.addTab("Audio Input", color, &_audioInputView, false);
    // _tabs.addTab("Generator", color, &_generatorTab, false);
    _tabs.addTab("First Reflections", color, _firstReflectionsView.get(), false);
    _tabs.addTab("Raytracing", color, std::addressof(_raytracing), false);
    _tabs.addTab("Porous Absorber", color, _absorberSimulationView.get(), false);
    if (tabIndex > 0 && tabIndex <= _tabs.getNumTabs()) {
        _tabs.setCurrentTabIndex(tabIndex, true);
    }
}

auto MainComponent::toggleFullscreen() -> void
{
    getPeer()->setFullScreen(!getPeer()->isFullScreen());
    // if (getPeer()->isFullScreen()) { }
}
}  // namespace ra
