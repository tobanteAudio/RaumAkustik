#pragma once

#include "tabs/room_editor.hpp"

#include <ra/acoustic/StochasticRaytracing.hpp>
#include <ra/acoustic/WaveEquation2D.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {

struct WaveEquation2DEditor final : juce::Component
{
    explicit WaveEquation2DEditor(RoomEditor& roomEditor);
    ~WaveEquation2DEditor() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

private:
    auto run() -> void;

    RoomEditor& _roomEditor;
    juce::ThreadPool _threadPool;

    juce::Value _duration{juce::var(2.0)};
    juce::Value _fmax{juce::var(2000.0)};
    juce::Value _ppw{juce::var(6.0)};

    juce::PropertyPanel _properties;
    juce::TextButton _render{"Render"};

    std::mutex _frameMutex;
    stdex::mdarray<double, stdex::dextents<size_t, 2>> _frame;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveEquation2DEditor)  // NOLINT
};

}  // namespace ra
