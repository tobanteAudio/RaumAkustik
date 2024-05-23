#include "WaveEquation2DEditor.hpp"

#include "look/color.hpp"

#include <neo/algorithm/copy.hpp>

#include <juce_audio_basics/juce_audio_basics.h>

#include <iostream>

namespace ra {

WaveEquation2DEditor::WaveEquation2DEditor(RoomEditor& roomEditor) : _roomEditor{roomEditor}
{
    using juce::SliderPropertyComponent;
    using std::make_unique;

    _properties.addProperties(juce::Array<juce::PropertyComponent*>{
        make_unique<SliderPropertyComponent>(_duration, "Duration", 0.5, 10.0, 0.1).release(),
        make_unique<SliderPropertyComponent>(_fmax, "Max Frequency", 200.0, 20'000.0, 1.0).release(),
        make_unique<SliderPropertyComponent>(_ppw, "PPW", 1.0, 10.0, 1.0).release(),
    });

    _render.onClick = [this] {
        _threadPool.addJob([this] { run(); });
        startTimerHz(30);
    };

    addAndMakeVisible(_properties);
    addAndMakeVisible(_render);
}

auto WaveEquation2DEditor::paint(juce::Graphics& g) -> void
{
    auto const area = getLocalBounds().withTrimmedRight(_render.getWidth()).reduced((4));

    g.setColour(juce::Colours::white);
    g.fillRect(area);

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
    }

    g.drawImage(_frameImage, area.toFloat(), juce::RectanglePlacement::centred);
}

auto WaveEquation2DEditor::resized() -> void
{
    auto area = getLocalBounds();

    auto panel = area.removeFromRight(area.proportionOfWidth(0.175));
    _properties.setBounds(panel.removeFromTop(panel.proportionOfHeight(0.9)));
    _render.setBounds(panel);
}

auto WaveEquation2DEditor::timerCallback() -> void { repaint(); }

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

    auto iterations = 0L;

    auto start = std::chrono::steady_clock::now();
    we([&](auto frame) {
        ++iterations;

        {
            auto lock = std::scoped_lock{_frameMutex};
            if (_frame.extents() != frame.extents()) {
                _frame = stdex::mdarray<double, stdex::dextents<size_t, 2>>{frame.extents()};
            }

            neo::copy(frame, _frame.to_mdspan());
        }
    });
    auto stop = std::chrono::steady_clock::now();

    auto const sec = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);
    std::cout << iterations << " iterations in " << sec.count() << " seconds\n";
    std::cout << 1000 * 1000 * iterations / sec.count() / 1'000'000.0 << " Megavoxel per seconds\n";

    juce::MessageManager::callAsync([this] { stopTimer(); });
}

}  // namespace ra
