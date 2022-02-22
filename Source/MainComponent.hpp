#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

struct MainComponent : juce::Component
{
    MainComponent();
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider _roomLengthSlider {juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight};
    juce::Slider _roomWidthSlider {juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight};
    juce::Slider _roomHeightSlider {juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight};

    juce::Value _roomLength {_roomLengthSlider.getValueObject()};
    juce::Value _roomWidth {_roomWidthSlider.getValueObject()};
    juce::Value _roomHeight {_roomHeightSlider.getValueObject()};

    juce::Rectangle<int> _drawArea {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
