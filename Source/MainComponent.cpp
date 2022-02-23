#include "MainComponent.hpp"

MainComponent::MainComponent()
{
    auto const color = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    _tabs.addTab("Room", color, &_roomSimulationView, false);
    _tabs.addTab("Porous Absorber", color, &_absorberSimulationView, false);
    addAndMakeVisible(_tabs);
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
    _tabs.setBounds(area);
}
