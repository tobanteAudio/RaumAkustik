#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "BinaryData.hpp"

struct MainComponent
    : juce::Component
    , juce::Value::Listener
{
    MainComponent();
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    auto connectValuesToTree() -> void;
    auto save() -> void;
    auto load() -> void;

    auto valueChanged(juce::Value& /*value*/) -> void override;

    juce::UndoManager _undoManager;
    juce::ValueTree _roomTree {"RaumAkustik"};

    juce::TextButton _load {"Load"};
    juce::TextButton _save {"Save"};

    std::unique_ptr<juce::Drawable> _speakerIcon {
        juce::Drawable::createFromImageData(mcbd::speaker_svg, mcbd::speaker_svgSize)};
    std::unique_ptr<juce::Drawable> _headIcon {
        juce::Drawable::createFromImageData(mcbd::insert_emoticon_svg, mcbd::insert_emoticon_svgSize)};

    juce::PropertyPanel _roomProperties {};

    juce::Value _iconSize {};

    juce::Value _roomLength {};
    juce::Value _roomWidth {};
    juce::Value _roomHeight {};

    juce::Value _listenX {};
    juce::Value _listenY {};
    juce::Value _listenZ {};

    juce::Value _leftX {};
    juce::Value _leftY {};
    juce::Value _leftZ {};

    juce::Value _rightX {};
    juce::Value _rightY {};
    juce::Value _rightZ {};

    juce::Rectangle<int> _drawArea {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
