#include "StochasticRaytracingEditor.hpp"

#include "tool/PropertyComponent.hpp"

#include <iostream>
#include <juce_audio_basics/juce_audio_basics.h>

namespace ra {

StochasticRaytracingEditor::StochasticRaytracingEditor(juce::ThreadPool& threadPool, RoomEditor& roomEditor)
    : _threadPool{threadPool}
    , _roomEditor{roomEditor}
{
    juce::ignoreUnused(_threadPool);

    _properties.addProperties(juce::Array<juce::PropertyComponent*>{
        makeProperty<juce::SliderPropertyComponent>(_rays, "Rays", 1'000.0, 100'000.0, 1.0),
        makeProperty<juce::SliderPropertyComponent>(_duration, "Duration", 1.0, 10.0, 0.1),
    });

    _render.onClick = [this] { run(); };

    addAndMakeVisible(_properties);
    addAndMakeVisible(_render);
    for (auto i{0}; i < 10; ++i) {
        addAndMakeVisible(_plots.add(std::make_unique<Plot>()));
    }
}

auto StochasticRaytracingEditor::paint(juce::Graphics& g) -> void
{
    auto plotBounds = [](auto* comp) { return comp->getBounds(); };
    auto getUnion   = [](auto l, auto r) { return l.getUnion(r); };
    auto plots      = std::transform_reduce(_plots.begin(), _plots.end(), juce::Rectangle<int>{}, getUnion, plotBounds);
    g.setColour(juce::Colours::white);
    g.fillRect(plots.expanded(8.0F));

    if (not _result) {
        return;
    }
}

auto StochasticRaytracingEditor::resized() -> void
{
    auto area = getLocalBounds();

    auto panel = area.removeFromRight(area.proportionOfWidth(0.175));
    _properties.setBounds(panel.removeFromTop(panel.proportionOfHeight(0.9)));
    _render.setBounds(panel);

    auto top   = area.removeFromTop(area.proportionOfHeight(0.5));
    auto width = area.proportionOfWidth(1.0 / 5.0);

    // top
    _plots[0]->setBounds(top.removeFromLeft(width).reduced(8.0F));
    _plots[1]->setBounds(top.removeFromLeft(width).reduced(8.0F));
    _plots[2]->setBounds(top.removeFromLeft(width).reduced(8.0F));
    _plots[3]->setBounds(top.removeFromLeft(width).reduced(8.0F));
    _plots[4]->setBounds(top.removeFromLeft(width).reduced(8.0F));

    // bottom
    _plots[5]->setBounds(area.removeFromLeft(width).reduced(8.0F));
    _plots[6]->setBounds(area.removeFromLeft(width).reduced(8.0F));
    _plots[7]->setBounds(area.removeFromLeft(width).reduced(8.0F));
    _plots[8]->setBounds(area.removeFromLeft(width).reduced(8.0F));
    _plots[9]->setBounds(area.removeFromLeft(width).reduced(8.0F));
}

auto StochasticRaytracingEditor::run() -> void
{
    using si::unit_symbols::Hz;

    auto const frequencies = std::vector<quantity<isq::frequency[si::hertz]>>{
        31.25 * Hz,
        62.5 * Hz,
        125.0 * Hz,
        250.0 * Hz,
        500.0 * Hz,
        1000.0 * Hz,
        2000.0 * Hz,
        4000.0 * Hz,
        8000.0 * Hz,
        16'000.0 * Hz,
    };

    auto const simulation = StochasticRaytracing::Simulation{
        .frequencies = frequencies,
        .duration    = static_cast<double>(_duration.getValue()) * si::second,
        .timeStep    = 0.001 * si::second,
        .radius      = 0.0875 * si::metre,
        .rays        = static_cast<size_t>(static_cast<double>(_rays.getValue())),
    };

    auto const roomLayout      = _roomEditor.getRoomLayout();
    auto const paintedConcrete = std::vector{0.01, 0.01, 0.01, 0.05, 0.06, 0.07, 0.09, 0.08, 0.08, 0.08};
    auto const woodFloor       = std::vector{0.15, 0.15, 0.15, 0.11, 0.1, 0.07, 0.06, 0.07, 0.07, 0.07};
    auto const absorption      = RoomAbsorption{
             .front   = paintedConcrete,
             .back    = paintedConcrete,
             .left    = paintedConcrete,
             .right   = paintedConcrete,
             .ceiling = paintedConcrete,
             .floor   = woodFloor,
    };
    auto const scattering = RoomScattering{
        .front   = {0.05, 0.05, 0.05,  0.3, 0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .back    = {0.05, 0.05, 0.05,  0.3, 0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .left    = {0.05, 0.05, 0.05,  0.3, 0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .right   = {0.05, 0.05, 0.05,  0.3, 0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .ceiling = {0.05, 0.05, 0.05,  0.3, 0.4, 0.5, 0.55, 0.6, 0.6, 0.6},
        .floor   = {0.01, 0.01, 0.01, 0.05, 0.1, 0.2,  0.3, 0.5, 0.5, 0.5},
    };

    auto const room = StochasticRaytracing::Room{
        .dimensions = roomLayout.dimensions,
        .absorption = absorption,
        .reflection = makeReflection(absorption),
        .scattering = scattering,
        .source     = roomLayout.speakers[0],
        .receiver   = roomLayout.listenPosition,
    };

    _threadPool.addJob([simulation, room, this] {
        auto raytracer = StochasticRaytracing{room};

        auto start  = std::chrono::steady_clock::now();
        auto result = raytracer(simulation);
        auto stop   = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count() << "s\n";

        juce::MessageManager::callAsync([simulation, result, this] {
            _result = std::move(result);
            for (auto const& frequency : *_result) {
                _maxGain = std::max(_maxGain, *std::max_element(frequency.begin(), frequency.end()));
            }

            for (auto i{0U}; i < _result->size(); ++i) {
                auto const f = simulation.frequencies[i].numerical_value_in(si::hertz);
                _plots[int(i)]->plot(juce::String(f) + "Hz", _result->at(i), simulation.duration, _maxGain);
            }

            repaint();
        });
    });
}

auto StochasticRaytracingEditor::Plot::plot(
    juce::String title,
    std::vector<double> const& data,
    quantity<isq::duration[si::second]> duration,
    double maxGain
) -> void
{
    _title    = std::move(title);
    _data     = data;
    _duration = duration;
    _maxGain  = maxGain;
    repaint();
}

auto StochasticRaytracingEditor::Plot::paint(juce::Graphics& g) -> void
{
    if (_data.empty()) {
        return;
    }

    auto area        = getLocalBounds().toFloat();
    auto const title = area.removeFromTop(std::max(16.0F, area.proportionOfHeight(0.1F)));

    auto const gainToY = [&](auto gain) {
        static constexpr auto const minDB = -80.0F;
        static constexpr auto const maxDB = 0.0F;

        auto const normalized = static_cast<float>(gain);
        auto const dB         = juce::Decibels::gainToDecibels(normalized, minDB);
        return juce::jmap(dB, minDB, maxDB, area.getBottom(), area.getY());
    };

    // outline
    g.setColour(juce::Colours::black);
    g.drawRect(area, 1.0F);

    // title
    g.setFont(juce::FontOptions{}.withPointHeight(14.0F));
    g.drawText(_title, title, juce::Justification::centred);

    // grid
    // dB
    auto const dBm10 = gainToY(juce::Decibels::decibelsToGain(-10.0));
    auto const dBm20 = gainToY(juce::Decibels::decibelsToGain(-20.0));
    auto const dBm30 = gainToY(juce::Decibels::decibelsToGain(-30.0));
    auto const dBm60 = gainToY(juce::Decibels::decibelsToGain(-60.0));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm10), area.getBottomRight().withY(dBm10)));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm20), area.getBottomRight().withY(dBm20)));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm30), area.getBottomRight().withY(dBm30)));
    g.drawLine(juce::Line<float>(area.getBottomLeft().withY(dBm60), area.getBottomRight().withY(dBm60)));

    // time
    auto const duration = _duration.numerical_value_in(si::second);
    auto const sec05    = juce::roundToInt(area.getX() + area.proportionOfWidth(0.5 / duration));
    auto const sec1     = juce::roundToInt(area.getX() + area.proportionOfWidth(1.0 / duration));
    auto const sec15    = juce::roundToInt(area.getX() + area.proportionOfWidth(1.5 / duration));
    g.drawVerticalLine(sec05, area.getY(), area.getBottom());
    g.drawVerticalLine(sec1, area.getY(), area.getBottom());
    g.drawVerticalLine(sec15, area.getY(), area.getBottom());

    // plot
    auto path = juce::Path{};
    path.startNewSubPath({0.0F, gainToY(_data[0] / _maxGain)});

    auto const deltaX = area.getWidth() / static_cast<float>(_data.size());
    for (auto i{1U}; i < _data.size(); ++i) {
        auto const x = area.getX() + deltaX * static_cast<float>(i);
        path.lineTo({x, gainToY(_data[i] / _maxGain)});
    }

    path.lineTo(area.getBottomRight());

    g.setColour(juce::Colours::black);
    g.strokePath(path, juce::PathStrokeType{1.0F, juce::PathStrokeType::curved});
}

}  // namespace ra
