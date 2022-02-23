#include "PorousAbsorberSimulationView.hpp"

#include "mc/RaumAkustik/Utility.hpp"

namespace mc
{

namespace
{
auto positionForFrequency(double const freq) noexcept -> double
{
    return (std::log(freq / 20.0) / std::log(2.0)) / 10.0;
}

}  // namespace

PorousAbsorberSimulationView::PorousAbsorberSimulationView()
{
    addAndMakeVisible(_table);
    addAndMakeVisible(_absorberSpecs);

    _table.getHeader().addColumn("Frequency", 1, 100, 100);
    _table.getHeader().addColumn("Reflection", 2, 100, 100);
    _table.getHeader().addColumn("Absorption", 3, 100, 100);

    _temperature.setValue(20.0);
    _pressure.setValue(1.0);

    _absorberThickness.setValue(100.0);
    _absorberFlowResisitivity.setValue(8000.0);
    _absorberAngleOfIncidence.setValue(0.0);

    _plotNumPoints.setValue(48.0);
    _plotStartFrequency.setValue(20.0);
    _plotOctaveSubdivision.setValue(6.0);

    _absorberSpecs.addSection("Atmospheric Environment",
                              juce::Array<juce::PropertyComponent*> {
                                  new juce::SliderPropertyComponent {_temperature, "Temperature (C)", 0.0, 100.0, 1.0},
                                  new juce::SliderPropertyComponent {_pressure, "Pressure (Bar)", 0.0, 2.0, 0.01},
                              });

    _absorberSpecs.addSection(
        "Absorber Dimensions",
        juce::Array<juce::PropertyComponent*> {
            new juce::SliderPropertyComponent {_absorberThickness, "Thickness (mm)", 0.0, 1000.0, 1.0},
            new juce::SliderPropertyComponent {_absorberFlowResisitivity, "Flow Resisitivity", 0.0, 10'000.0, 1.0},
            new juce::SliderPropertyComponent {_absorberAngleOfIncidence, "Angle Of Incidence", 0.0, 90.0, 1.0},

        });

    _absorberSpecs.addSection(
        "Plot", juce::Array<juce::PropertyComponent*> {
                    new juce::SliderPropertyComponent {_plotNumPoints, "Num Points", 0.0, 256.0, 1.0},
                    new juce::SliderPropertyComponent {_plotStartFrequency, "Start Frequency", 0.0, 60.0, 1.0},
                    new juce::SliderPropertyComponent {_plotOctaveSubdivision, "Octave Subdivisions", 0.0, 12.0, 1.0},

                });

    _temperature.addListener(this);
    _pressure.addListener(this);
    _absorberThickness.addListener(this);
    _absorberFlowResisitivity.addListener(this);
    _absorberAngleOfIncidence.addListener(this);
    _plotNumPoints.addListener(this);
    _plotStartFrequency.addListener(this);
    _plotOctaveSubdivision.addListener(this);

    valueChanged(_temperature);
}

auto PorousAbsorberSimulationView::paint(juce::Graphics& g) -> void
{
    juce::Graphics::ScopedSaveState state {g};

    g.reduceClipRegion(_plotArea);

    g.setColour(juce::Colours::white);
    g.fillRect(_plotArea);

    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    for (auto freq : std::vector {20.0, 40.0, 80.0, 160.0, 320.0, 640.0, 1280.0, 2560.0, 5120.0})
    {
        auto x = static_cast<float>(_plotArea.getX())
                 + static_cast<float>(_plotArea.getWidth()) * static_cast<float>(positionForFrequency(freq));
        auto const topY    = static_cast<float>(_plotArea.getY());
        auto const bottomY = static_cast<float>(_plotArea.getBottom());
        g.drawLine(x, topY, x, bottomY, 3.0f);
    }

    auto path = juce::Path {};
    path.startNewSubPath(_plotArea.getBottomLeft().toFloat());
    for (auto const& p : _props)
    {
        auto posX = _plotArea.getX() + _plotArea.getWidth() * positionForFrequency(p.first);
        auto posY = _plotArea.getBottom() - _plotArea.getHeight() * p.second.absorptionFactorNoAirGap;
        path.lineTo(juce::Point {posX, posY}.toFloat());
    }

    path = path.createPathWithRoundedCorners(5.0f);
    g.setColour(juce::Colours::black);
    g.strokePath(path, juce::PathStrokeType {2.0f});
}

auto PorousAbsorberSimulationView::resized() -> void
{
    auto area = getLocalBounds();
    _absorberSpecs.setBounds(area.removeFromTop(area.proportionOfHeight(0.3)));

    _plotArea = area.removeFromLeft(area.proportionOfWidth(0.5)).reduced(10);
    _table.setBounds(area.reduced(10));
}

auto PorousAbsorberSimulationView::valueChanged(juce::Value& /*value*/) -> void
{
    _props.clear();

    auto const specs = PorousAbsorberSpecs {
        static_cast<double>(_absorberThickness.getValue()),
        static_cast<double>(_absorberFlowResisitivity.getValue()),
    };

    auto const env = AtmosphericEnvironment {
        static_cast<double>(_temperature.getValue()),
        static_cast<double>(_pressure.getValue()),
    };

    auto const angle = static_cast<double>(_absorberAngleOfIncidence.getValue());

    auto const startFrequency = static_cast<double>(_plotStartFrequency.getValue());
    auto const subDivisions   = static_cast<double>(_plotOctaveSubdivision.getValue());

    for (auto i {0}; i < static_cast<int>(_plotNumPoints.getValue()); ++i)
    {
        auto const frequency = oactaveSubdivision(startFrequency, static_cast<size_t>(subDivisions), i);
        _props.push_back(std::make_pair(frequency, propertiesOfAbsorber(specs, env, frequency, angle)));
    }

    _table.updateContent();
    repaint();
}

auto PorousAbsorberSimulationView::getNumRows() -> int { return static_cast<int>(_props.size()); }

auto PorousAbsorberSimulationView::paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/,
                                                      bool rowIsSelected) -> void
{
    auto alternateColour = getLookAndFeel()
                               .findColour(juce::ListBox::backgroundColourId)
                               .interpolatedWith(getLookAndFeel().findColour(juce::ListBox::textColourId), 0.03f);
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}

auto PorousAbsorberSimulationView::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height,
                                             bool /*rowIsSelected*/) -> void
{
    g.setColour(getLookAndFeel().findColour(juce::ListBox::textColourId));
    g.setFont(16.0f);

    if (columnId == 1)
    {
        auto const absorptionFactor = juce::String {_props[rowNumber].first};
        g.drawText(absorptionFactor, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    if (columnId == 2)
    {
        auto const reflectionFactor = juce::String {_props[rowNumber].second.reflectionFactorNoAirGap.real()};
        g.drawText(reflectionFactor, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    if (columnId == 3)
    {
        auto const absorptionFactor = juce::String {_props[rowNumber].second.absorptionFactorNoAirGap};
        g.drawText(absorptionFactor, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    g.fillRect(width - 1, 0, 1, height);
}
}  // namespace mc