#include "MainComponent.hpp"

MainComponent::MainComponent()
{
    addAndMakeVisible(_undo);
    addAndMakeVisible(_redo);
    addAndMakeVisible(_load);
    addAndMakeVisible(_save);
    addAndMakeVisible(_tabs);

    _undo.onClick = [this] { _undoManager.undo(); };
    _redo.onClick = [this] { _undoManager.redo(); };
    _save.onClick = [this] { saveSession(); };
    _load.onClick = [this] { loadSession(); };

    _undo.setImages(juce::Drawable::createFromImageData(mcbd::undo_svg, mcbd::undo_svgSize).get());
    _redo.setImages(juce::Drawable::createFromImageData(mcbd::redo_svg, mcbd::redo_svgSize).get());
    _save.setImages(juce::Drawable::createFromImageData(mcbd::save_svg, mcbd::save_svgSize).get());
    _load.setImages(juce::Drawable::createFromImageData(mcbd::folder_open_svg, mcbd::folder_open_svgSize).get());

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
    auto area = getLocalBounds().reduced(10);

    auto buttonArea        = area.removeFromTop(area.proportionOfHeight(0.1));
    auto const buttonWidth = buttonArea.proportionOfWidth(0.1);
    _undo.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    _redo.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    _load.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    _save.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));

    _tabs.setBounds(area);
}

auto MainComponent::saveSession() -> void
{
    auto file = juce::File {juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    if (file.existsAsFile()) { file.deleteFile(); }
    auto stream = juce::FileOutputStream {file};
    _valueTree.writeToStream(stream);
}

auto MainComponent::loadSession() -> void
{
    auto file   = juce::File {juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    auto stream = juce::FileInputStream {file};
    _valueTree  = juce::ValueTree::readFromStream(stream);
    reloadUI();
}

auto MainComponent::reloadUI() -> void
{
    _tabs.clearTabs();

    _firstReflectionsView   = std::make_unique<mc::FirstReflectionsView>(_valueTree);
    _absorberSimulationView = std::make_unique<mc::PorousAbsorberSimulationView>(_valueTree);

    auto const color = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    _tabs.addTab("First Reflections", color, _firstReflectionsView.get(), false);
    _tabs.addTab("Porous Absorber", color, _absorberSimulationView.get(), false);
}