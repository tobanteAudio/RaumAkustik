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

    _render.onClick = [this] { _threadPool.addJob([this] { run(); }); };

    addAndMakeVisible(_properties);
    addAndMakeVisible(_render);
}

auto WaveEquation2DEditor::paint(juce::Graphics& g) -> void
{
    g.setColour(juce::Colours::white);
    g.fillRect(getLocalBounds().withTrimmedRight(_render.getWidth()).reduced((4)));

    {
        auto lock = std::scoped_lock{_frameMutex};

        auto const width = 2;
        for (auto x{0U}; x < _frame.extent(0); ++x) {
            for (auto y{0U}; y < _frame.extent(1); ++y) {
                auto const val = _frame(x, y);
                auto const dB  = juce::Decibels::gainToDecibels(val);

                // auto const t     = juce::jmap(dB, -100.0, 0.0, 0.0, 1.0);
                // auto const color = juce::Colours::black.withAlpha(static_cast<float>(t));

                auto const t     = size_t(juce::jmap(dB, -100.0, 0.0, 0.0, double(CividisColorMap.size() - 1U)));
                auto const color = CividisColorMap[std::clamp(t, size_t(0), CividisColorMap.size() - 1U)];

                auto const area = juce::Rectangle{int(x * width), int(y * width), width, width};
                g.setColour(color);
                g.fillRect(area);
            }
        }
    }
}

auto WaveEquation2DEditor::resized() -> void
{
    auto area = getLocalBounds();

    auto panel = area.removeFromRight(area.proportionOfWidth(0.175));
    _properties.setBounds(panel.removeFromTop(panel.proportionOfHeight(0.9)));
    _render.setBounds(panel);
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

    auto iterations = 0L;

    auto start = std::chrono::steady_clock::now();
    we([&](auto frame) {
        ++iterations;

        // if (iterations % 50 != 0) {
        //     return;
        // }

        {
            auto lock = std::scoped_lock{_frameMutex};
            if (_frame.extents() != frame.extents()) {
                _frame = stdex::mdarray<double, stdex::dextents<size_t, 2>>{frame.extents()};
            }

            neo::copy(frame, _frame.to_mdspan());
        }

        juce::MessageManager::callAsync([this] { repaint(); });
    });
    auto stop = std::chrono::steady_clock::now();

    auto const sec = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);
    std::cout << iterations << " iterations in " << sec.count() << " seconds\n";
    std::cout << 1000 * 1000 * iterations / sec.count() / 1'000'000.0 << " Megavoxel per seconds\n";
}

}  // namespace ra
