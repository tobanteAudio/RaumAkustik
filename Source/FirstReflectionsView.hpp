#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "BinaryData.hpp"

namespace mc
{
struct FirstReflectionsView final
    : juce::Component
    , juce::ValueTree::Listener
{
    FirstReflectionsView(juce::ValueTree vt);
    ~FirstReflectionsView() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    auto valueTreePropertyChanged(juce::ValueTree& tree, juce::Identifier const& property) -> void override;
    auto connectValuesToTree() -> void;

    juce::UndoManager _undoManager;
    juce::ValueTree _roomTree;

    juce::CachedValue<double> _iconSize {_roomTree, "icon_size", &_undoManager};

    juce::CachedValue<double> _roomLength {_roomTree, "room_length", &_undoManager};
    juce::CachedValue<double> _roomWidth {_roomTree, "room_width", &_undoManager};
    juce::CachedValue<double> _roomHeight {_roomTree, "room_height", &_undoManager};

    juce::CachedValue<double> _listenX {_roomTree, "listen_x", &_undoManager};
    juce::CachedValue<double> _listenY {_roomTree, "listen_y", &_undoManager};
    juce::CachedValue<double> _listenZ {_roomTree, "listen_z", &_undoManager};

    juce::CachedValue<double> _leftX {_roomTree, "left_x", &_undoManager};
    juce::CachedValue<double> _leftY {_roomTree, "left_y", &_undoManager};
    juce::CachedValue<double> _leftZ {_roomTree, "left_z", &_undoManager};

    juce::CachedValue<double> _rightX {_roomTree, "right_x", &_undoManager};
    juce::CachedValue<double> _rightY {_roomTree, "right_y", &_undoManager};
    juce::CachedValue<double> _rightZ {_roomTree, "right_z", &_undoManager};

    juce::CachedValue<bool> _renderLeftReflections {_roomTree, "render_left_reflections", &_undoManager};
    juce::CachedValue<bool> _renderRightReflections {_roomTree, "render_right_reflections", &_undoManager};

    std::unique_ptr<juce::Drawable> _speakerIcon {
        juce::Drawable::createFromImageData(mcbd::speaker_svg, mcbd::speaker_svgSize)};
    std::unique_ptr<juce::Drawable> _headIcon {
        juce::Drawable::createFromImageData(mcbd::insert_emoticon_svg, mcbd::insert_emoticon_svgSize)};

    juce::PropertyPanel _roomProperties {};
    juce::PropertyPanel _renderProperties {};

    juce::Rectangle<int> _drawArea {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FirstReflectionsView)
};
}  // namespace mc