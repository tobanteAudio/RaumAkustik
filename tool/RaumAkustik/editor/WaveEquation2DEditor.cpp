#include "WaveEquation2DEditor.hpp"

#include "look/ColorMap.hpp"
#include "tool/PropertyComponent.hpp"

#include <neo/algorithm/copy.hpp>

#include <juce_audio_basics/juce_audio_basics.h>
#include <neo_core/neo_core.hpp>

#include <iostream>

namespace ra {

WaveEquation2DEditor::WaveEquation2DEditor(juce::ThreadPool& threadPool, RoomEditor& roomEditor)
    : _threadPool{threadPool}
    , _roomEditor{roomEditor}
{
    using juce::SliderPropertyComponent;

    _title.setJustificationType(juce::Justification::centred);
    _title.setFont(juce::FontOptions().withPointHeight(16.0));
    _title.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::white);
    _title.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    _title.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::transparentBlack);

    _properties.addProperties(juce::Array<juce::PropertyComponent*>{
        makeProperty<SliderPropertyComponent>(_duration, "Duration", 0.5, 10.0, 0.1),
        makeProperty<SliderPropertyComponent>(_fmax, "Max Frequency", 200.0, 20'000.0, 1.0),
        makeProperty<SliderPropertyComponent>(_ppw, "PPW", 1.0, 10.0, 1.0),
    });

    _render.onClick = [this] {
        _readOut.clear();
        _threadPool.addJob([this] { run(); });
        startTimerHz(30);
    };

    addAndMakeVisible(_title);
    addAndMakeVisible(_properties);
    addAndMakeVisible(_render);
}

auto WaveEquation2DEditor::paint(juce::Graphics& g) -> void
{
    g.setColour(juce::Colours::white);
    g.fillRect(_plot);

    auto plot       = _plot;
    auto imgArea    = plot.removeFromLeft(plot.proportionOfWidth(0.5)).reduced(4.0F);
    auto signalArea = plot.reduced(4.0F, plot.proportionOfHeight(0.2));

    auto path = juce::Path{};
    {
        auto lock = std::scoped_lock{_frameMutex};

        auto const xExtent = static_cast<int>(_frame.extent(0));
        auto const yExtent = static_cast<int>(_frame.extent(1));
        if (_frameImage.getWidth() != xExtent or _frameImage.getHeight() != yExtent) {
            _frameImage = juce::Image{juce::Image::ARGB, xExtent, yExtent, true};
        }

        auto max = 0.0;
        for (auto x{0}; x < xExtent; ++x) {
            for (auto y{0}; y < yExtent; ++y) {
                auto const val = std::abs(_frame(x, y));
                max            = std::max(max, val);
            }
        }

        for (auto x{0}; x < xExtent; ++x) {
            for (auto y{0}; y < yExtent; ++y) {
                auto const val = _frame(x, y);

                auto const t     = size_t(juce::jmap(val, -max, max, 0.0, double(BondColorMap.size() - 1U)));
                auto const color = BondColorMap[std::clamp(t, size_t(0), BondColorMap.size() - 1U)];

                _frameImage.setPixelAt(x, y, color);
            }
        }

        if (not _readOut.empty()) {
            auto deltaX  = signalArea.getWidth() / static_cast<float>(_readOut.size());
            auto absLess = [](auto l, auto r) { return std::abs(l) < std::abs(r); };
            auto peak    = static_cast<float>(std::abs(*std::max_element(_readOut.begin(), _readOut.end(), absLess)));

            path.startNewSubPath(signalArea.getBottomLeft().withY(signalArea.getCentreY()));
            for (auto i{0U}; i < _readOut.size(); ++i) {
                auto val = static_cast<float>(_readOut[i]);
                auto x   = signalArea.getX() + deltaX * static_cast<float>(i);
                auto y   = juce::jmap(val, -peak, peak, signalArea.getBottom(), signalArea.getY());
                path.lineTo({x, y});
            }
        }
    }

    g.setColour(juce::Colours::black);
    g.strokePath(path, juce::PathStrokeType(1.0F));
    g.drawImage(_frameImage, imgArea, juce::RectanglePlacement::centred);
}

auto WaveEquation2DEditor::resized() -> void
{
    auto area = getLocalBounds();

    auto panel = area.removeFromRight(area.proportionOfWidth(0.175));
    _properties.setBounds(panel.removeFromTop(panel.proportionOfHeight(0.9)));
    _render.setBounds(panel);

    _title.setBounds(area.removeFromTop(area.proportionOfHeight(0.05)));
    _plot = area.toFloat();
}

auto WaveEquation2DEditor::timerCallback() -> void
{
    if (std::exchange(_needsRepaint, false)) {
        repaint();
    }
}

auto WaveEquation2DEditor::run() -> void
{
    auto const room = _roomEditor.getRoomLayout().dimensions;

    auto we = WaveEquation2D({
        .Lx       = room.width,
        .Ly       = room.length,
        .duration = static_cast<double>(_duration.getValue()) * si::second,
        .fmax     = static_cast<double>(_fmax.getValue()) * si::hertz,
        .ppw      = static_cast<double>(_ppw.getValue()),
    });

    auto iterations = 0UL;

    auto start = std::chrono::steady_clock::now();
    we([&](auto frame) {
        ++iterations;

        {
            auto lock = std::scoped_lock{_frameMutex};
            if (_frame.extents() != frame.extents()) {
                _frame = stdex::mdarray<double, stdex::dextents<size_t, 2>>{frame.extents()};
            }

            _readOut.push_back(_frame(_frame.extent(0) / 2U, _frame.extent(1) / 2U));
            neo::copy(frame, _frame.to_mdspan());
            _needsRepaint = true;
        }
    });
    auto stop = std::chrono::steady_clock::now();

    juce::MessageManager::callAsync([this, t = stop - start, iterations] {
        auto const sec  = std::chrono::duration_cast<std::chrono::duration<double>>(t).count();
        auto const x    = _frame.extent(0);
        auto const y    = _frame.extent(1);
        auto const mvox = x * y * iterations / sec / 1'000'000.0;

        _title.setText(
            neo::jformat("Grid: {}x{} with {} frames in {:.2f} s ({:.2f} Mvox/s)", x, y, iterations, sec, mvox),
            juce::sendNotification
        );

        stopTimer();
        repaint();
    });
}

}  // namespace ra
