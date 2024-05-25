#pragma once

#include "editor/RoomEditor.hpp"

#include <ra/acoustic/StochasticRaytracing.hpp>
#include <ra/acoustic/WaveEquation2D.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {

struct WaveEquation2DEditor final
    : juce::Component
    , juce::Timer
{
    WaveEquation2DEditor(juce::ThreadPool& threadPool, RoomEditor& roomEditor);
    ~WaveEquation2DEditor() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;
    auto timerCallback() -> void override;

private:
    auto run() -> void;

    juce::ThreadPool& _threadPool;
    RoomEditor& _roomEditor;

    juce::Value _duration{juce::var(2.0)};
    juce::Value _fmax{juce::var(2000.0)};
    juce::Value _ppw{juce::var(6.0)};

    juce::PropertyPanel _properties;
    juce::TextButton _render{"Render"};

    std::mutex _frameMutex;
    stdex::mdarray<double, stdex::dextents<size_t, 2>> _frame;

    juce::Image _frameImage{juce::Image::ARGB, 1, 1, true};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveEquation2DEditor)  // NOLINT
};

}  // namespace ra
