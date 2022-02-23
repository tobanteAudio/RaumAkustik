#include "MainComponent.hpp"

MainComponent::MainComponent()
{
    addAndMakeVisible(_roomSimulationView);
    setSize(600, 400);
}

auto MainComponent::paint(juce::Graphics& g) -> void
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

auto MainComponent::resized() -> void
{
    auto area = getLocalBounds().reduced(10);
    _roomSimulationView.setBounds(area);
}
