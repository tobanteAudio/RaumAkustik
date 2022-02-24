#include "MainComponent.hpp"

MainComponent::MainComponent()
{
    addAndMakeVisible(_menuBar);
    addAndMakeVisible(_tabs);

    _commandManager.registerAllCommandsForTarget(this);
    addKeyListener(_commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);

    reloadUI();
    setSize(1280, 720);
}

MainComponent::~MainComponent() { DBG(_valueTree.toXmlString()); }

auto MainComponent::paint(juce::Graphics& g) -> void
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

auto MainComponent::resized() -> void
{
    auto area = getLocalBounds();
    _menuBar.setBounds(area.removeFromTop(juce::LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
    _tabs.setBounds(area.reduced(10));
}

auto MainComponent::getNextCommandTarget() -> juce::ApplicationCommandTarget*
{
    return findFirstTargetParentComponent();
}

auto MainComponent::getAllCommands(juce::Array<juce::CommandID>& c) -> void
{
    c.addArray({CommandIDs::redo, CommandIDs::undo, CommandIDs::open, CommandIDs::save});
}

auto MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) -> void
{
    switch (commandID)
    {
        case CommandIDs::open:
            result.setInfo("Open", "Opens a session file", "File", 0);
            result.addDefaultKeypress('o', juce::ModifierKeys::commandModifier);
            break;
        case CommandIDs::save:
            result.setInfo("Save", "Saves a session file", "File", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
            break;
        case CommandIDs::undo:
            result.setInfo("Undo", "Undo one step", "Edit", 0);
            result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier);
            break;
        case CommandIDs::redo:
            result.setInfo("Redo", "Redo one step", "Edit", 0);
            result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
            break;
        default: break;
    }
}
auto MainComponent::perform(juce::ApplicationCommandTarget::InvocationInfo const& info) -> bool
{
    switch (info.commandID)
    {
        case CommandIDs::open: loadSession(); break;
        case CommandIDs::save: saveSession(); break;
        case CommandIDs::undo: _undoManager.undo(); break;
        case CommandIDs::redo: _undoManager.redo(); break;
        default: return false;
    }

    return true;
}

auto MainComponent::saveSession() -> void
{
    auto file = juce::File {juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    if (file.existsAsFile()) { file.deleteFile(); }
    auto stream = juce::FileOutputStream {file};
    stream.writeText(_valueTree.toXmlString(), false, false, "\n");
}

auto MainComponent::loadSession() -> void
{
    auto file   = juce::File {juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    auto stream = juce::FileInputStream {file};
    _valueTree  = juce::ValueTree::fromXml(stream.readEntireStreamAsString());
    reloadUI();
}

auto MainComponent::reloadUI() -> void
{
    _tabs.clearTabs();

    _firstReflectionsView   = std::make_unique<mc::FirstReflectionsView>(_valueTree, &_undoManager);
    _absorberSimulationView = std::make_unique<mc::PorousAbsorberSimulationView>(_valueTree, &_undoManager);

    auto const color = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    _tabs.addTab("First Reflections", color, _firstReflectionsView.get(), false);
    _tabs.addTab("Porous Absorber", color, _absorberSimulationView.get(), false);
}