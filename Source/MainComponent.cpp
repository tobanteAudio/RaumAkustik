#include "MainComponent.hpp"

MainComponent::MainComponent()
{
    auto const color = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    _tabs.addTab("Room Simulator", color, &_roomSimulationView, false);
    _tabs.addTab("Absorber Simulator", color, &_absorberSimulationView, false);
    addAndMakeVisible(_tabs);
    setSize(600, 400);
}

auto MainComponent::paint(juce::Graphics& g) -> void
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

auto MainComponent::resized() -> void
{
    auto area = getLocalBounds().reduced(10);
    _tabs.setBounds(area);
}
