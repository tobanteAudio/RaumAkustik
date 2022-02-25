#include "MainComponent.hpp"

#include "CommandIDs.hpp"

MainComponent::MainComponent()
{
    addAndMakeVisible(_menuBar);
    addAndMakeVisible(_tabs);

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
    setLookAndFeel(nullptr);
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

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
    c.addArray({
        mc::CommandIDs::open,
        mc::CommandIDs::save,
        mc::CommandIDs::saveAs,
        mc::CommandIDs::redo,
        mc::CommandIDs::undo,
        mc::CommandIDs::gotoTabLeft,
        mc::CommandIDs::gotoTabRight,
        mc::CommandIDs::about,
    });
}

auto MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) -> void
{
    using juce::KeyPress;
    using juce::ModifierKeys;

    switch (commandID)
    {
        case mc::CommandIDs::open:
            result.setInfo("Open", "Opens a project file", "File", 0);
            result.addDefaultKeypress('o', ModifierKeys::commandModifier);
            break;
        case mc::CommandIDs::save:
            result.setInfo("Save", "Saves a project file", "File", 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            break;
        case mc::CommandIDs::saveAs:
            result.setInfo("Save As", "Saves a project file to a new location", "File", 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            break;
        case mc::CommandIDs::undo:
            result.setInfo("Undo", "Undo one step", "Edit", 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier);
            break;
        case mc::CommandIDs::redo:
            result.setInfo("Redo", "Redo one step", "Edit", 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            break;
        case mc::CommandIDs::gotoTabLeft:
            result.setInfo("Goto tab left", "Goto tab on the left", "View", 0);
            result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::commandModifier);
            break;
        case mc::CommandIDs::gotoTabRight:
            result.setInfo("Goto tab right", "Goto tab on the right", "View", 0);
            result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            break;
        case mc::CommandIDs::about:
            result.setInfo("About", "Open about page", "Help", 0);
            result.addDefaultKeypress('h', ModifierKeys::commandModifier);
            break;
        default: break;
    }
}
auto MainComponent::perform(juce::ApplicationCommandTarget::InvocationInfo const& info) -> bool
{
    switch (info.commandID)
    {
        case mc::CommandIDs::open: loadProject(); break;
        case mc::CommandIDs::save: saveProject(); break;
        case mc::CommandIDs::saveAs: saveProject(); break;
        case mc::CommandIDs::undo: _undoManager.undo(); break;
        case mc::CommandIDs::redo: _undoManager.redo(); break;
        case mc::CommandIDs::about: showAboutMessage(); break;
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
    auto file = juce::File {juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    if (file.existsAsFile()) { file.deleteFile(); }
    auto stream = juce::FileOutputStream {file};
    stream.writeText(_valueTree.toXmlString(), false, false, "\n");
}

auto MainComponent::loadProject() -> void
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