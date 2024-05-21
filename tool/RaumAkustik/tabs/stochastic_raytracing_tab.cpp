#include "stochastic_raytracing_tab.hpp"

#include <iostream>
#include <juce_audio_basics/juce_audio_basics.h>

namespace ra {

StochasticRaytracingEditor::StochasticRaytracingEditor(RoomEditor& roomEditor) : _roomEditor{roomEditor}
{
    _properties.addProperties(juce::Array<juce::PropertyComponent*>{
        std::make_unique<juce::SliderPropertyComponent>(_rays, "Rays", 1'000.0, 100'000.0, 1.0).release(),
        std::make_unique<juce::SliderPropertyComponent>(_duration, "Duration", 1.0, 10.0, 0.1).release(),
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
    auto const simulation = StochasticRaytracing::Simulation{
        .frequencies = std::vector{31.25, 62.5, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0, 16'000.0},
        .rays        = static_cast<size_t>(static_cast<double>(_rays.getValue())),
        .duration    = std::chrono::duration<double>{static_cast<double>(_duration.getValue())},
        .timeStep    = std::chrono::duration<double>{0.001},
        .radius      = 0.0875,
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

    auto raytracer = StochasticRaytracing{room};

    auto start = std::chrono::steady_clock::now();
    _result    = raytracer(simulation);
    auto stop  = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count() << "s\n";

    for (auto const& frequency : *_result) {
        _maxGain = std::max(_maxGain, *std::max_element(frequency.begin(), frequency.end()));
    }

    for (auto i{0U}; i < _result->size(); ++i) {
        _plots[static_cast<int>(i)]
            ->plot(juce::String(simulation.frequencies[i]) + "Hz", _result->at(i), simulation.duration, _maxGain);
    }

    repaint();
}

auto StochasticRaytracingEditor::Plot::plot(
    juce::String title,
    std::vector<double> const& data,
    std::chrono::duration<double> duration,
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
    g.setFont(juce::Font{}.withPointHeight(14.0F));
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
    auto const sec05 = juce::roundToInt(area.getX() + area.proportionOfWidth(0.5 / _duration.count()));
    auto const sec1  = juce::roundToInt(area.getX() + area.proportionOfWidth(1.0 / _duration.count()));
    auto const sec15 = juce::roundToInt(area.getX() + area.proportionOfWidth(1.5 / _duration.count()));
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