#include "absorber_simulation_tab.hpp"

#include <ra/unit/frequency.hpp>
#include <ra/unit/pressure.hpp>
#include <ra/unit/temperature.hpp>

namespace ra {

namespace {
auto positionForFrequency(quantity<isq::frequency[si::hertz]> const freq) noexcept -> double
{
    return (std::log(freq.numerical_value_in(si::hertz) / 20.0) / std::numbers::ln2) / 10.0;
}

}  // namespace

PorousAbsorberSimulationEditor::PorousAbsorberSimulationEditor(juce::ValueTree vt, juce::UndoManager* um)
    : _undoManager{um}
    , _valueTree{vt.getOrCreateChildWithName("PorousAbsorber", um)}
{
    addAndMakeVisible(_table);
    addAndMakeVisible(_absorberSpecs);

    _table.getHeader().addColumn("Frequency", 1, 100, 100);
    _table.getHeader().addColumn("Absorption No Gap", 2, 150, 150);
    _table.getHeader().addColumn("Absorption With Gap", 3, 150, 150);

    _absorberSpecs.addSection(
        "Atmospheric Environment",
        juce::Array<juce::PropertyComponent*>{
            new juce::SliderPropertyComponent{_temperature.getPropertyAsValue(), "Temperature (C)", 0.0, 100.0, 1.0 },
            new juce::SliderPropertyComponent{_pressure.getPropertyAsValue(),    "Pressure (Bar)",  0.0, 2.0,   0.01},
    }
    );

    _absorberSpecs.addSection(
        "Absorber Dimensions",
        juce::Array<juce::PropertyComponent*>{
            new juce::SliderPropertyComponent{
                                              _absorberThickness.getPropertyAsValue(),
                                              "Thickness (mm)",                                               0.0,
                                              1000.0,                                                                      1.0
            },
            new juce::SliderPropertyComponent{
                                              _absorberFlowResisitivity.getPropertyAsValue(),
                                              "Flow Resisitivity",                                            0.0,
                                              40'000.0,                                                                    1.0
            },
            new juce::SliderPropertyComponent{
                                              _absorberAngleOfIncidence.getPropertyAsValue(),
                                              "Angle Of Incidence",                                           0.0,
                                              90.0,                                                                        1.0
            },
            new juce::SliderPropertyComponent{_absorberAirGap.getPropertyAsValue(),           "Air Gap (mm)", 0.0, 1000.0, 1.0},
    }
    );

    _absorberSpecs.addSection(
        "Plot",
        juce::Array<juce::PropertyComponent*>{
            new juce::SliderPropertyComponent{_plotNumPoints.getPropertyAsValue(),         "Num Points", 0.0, 256.0, 1.0},
            new juce::SliderPropertyComponent{
                                              _plotStartFrequency.getPropertyAsValue(),
                                              "Start Frequency",                                         0.0,
                                              60.0,                                                                  1.0
            },
            new juce::SliderPropertyComponent{
                                              _plotOctaveSubdivision.getPropertyAsValue(),
                                              "Octave Subdivisions",                                     0.0,
                                              12.0,                                                                  1.0
            },
    }
    );

    _temperature.getValueTree().addListener(this);
    _pressure.getValueTree().addListener(this);
    _absorberThickness.getValueTree().addListener(this);
    _absorberFlowResisitivity.getValueTree().addListener(this);
    _absorberAngleOfIncidence.getValueTree().addListener(this);
    _absorberAirGap.getValueTree().addListener(this);
    _plotNumPoints.getValueTree().addListener(this);
    _plotStartFrequency.getValueTree().addListener(this);
    _plotOctaveSubdivision.getValueTree().addListener(this);

    updateSimulation();
}

auto PorousAbsorberSimulationEditor::paint(juce::Graphics& g) -> void
{
    juce::Graphics::ScopedSaveState const state{g};

    g.reduceClipRegion(_plotArea);

    g.setColour(juce::Colours::white);
    g.fillRect(_plotArea);

    g.setColour(juce::Colours::grey.withAlpha(0.5F));
    for (auto freq : std::array{31.25, 62.50, 125.0, 250.0, 500.0, 1'000.0, 2'000.0, 4'000.0, 8'000.0}) {
        auto const freqPos = static_cast<float>(positionForFrequency(freq * si::hertz));
        auto const x       = static_cast<float>(_plotArea.getX()) + static_cast<float>(_plotArea.getWidth()) * freqPos;
        auto const topY    = static_cast<float>(_plotArea.getY());
        auto const bottomY = static_cast<float>(_plotArea.getBottom());
        g.drawLine(x, topY, x, bottomY, 3.0F);
    }

    for (auto row : std::array{0.2F, 0.4F, 0.6F, 0.8F}) {
        auto const y = juce::jmap(row, _plotArea.toFloat().getY(), _plotArea.toFloat().getBottom());
        g.drawHorizontalLine(juce::roundToInt(y), _plotArea.toFloat().getX(), _plotArea.toFloat().getRight());
    }

    auto noAirGapPath = juce::Path{};
    noAirGapPath.startNewSubPath(_plotArea.getBottomLeft().toFloat());

    auto withAirGapPath = juce::Path{};
    withAirGapPath.startNewSubPath(_plotArea.getBottomLeft().toFloat());

    for (auto const& p : _props) {
        auto posX     = _plotArea.getX() + _plotArea.getWidth() * positionForFrequency(p.first);
        auto noGapY   = _plotArea.getBottom() - _plotArea.getHeight() * p.second.absorptionFactorNoAirGap;
        auto withGapY = _plotArea.getBottom() - _plotArea.getHeight() * p.second.absorptionFactorWithAirGap;
        noAirGapPath.lineTo(juce::Point{posX, noGapY}.toFloat());
        withAirGapPath.lineTo(juce::Point{posX, withGapY}.toFloat());
    }

    noAirGapPath = noAirGapPath.createPathWithRoundedCorners(5.0F);
    g.setColour(juce::Colours::black);
    g.strokePath(noAirGapPath, juce::PathStrokeType{2.0F});

    withAirGapPath = withAirGapPath.createPathWithRoundedCorners(5.0F);
    g.setColour(juce::Colours::red);
    g.strokePath(withAirGapPath, juce::PathStrokeType{2.0F});
}

auto PorousAbsorberSimulationEditor::resized() -> void
{
    auto area = getLocalBounds();
    _absorberSpecs.setBounds(area.removeFromTop(area.proportionOfHeight(0.4)));

    _plotArea = area.removeFromLeft(area.proportionOfWidth(0.5)).reduced(10);
    _table.setBounds(area.reduced(10));
}

auto PorousAbsorberSimulationEditor::
    valueTreePropertyChanged(juce::ValueTree& /*tree*/, juce::Identifier const& /*property*/) -> void
{
    updateSimulation();
}

auto PorousAbsorberSimulationEditor::getNumRows() -> int { return static_cast<int>(_props.size()); }

auto PorousAbsorberSimulationEditor::paintRowBackground(
    juce::Graphics& g,
    int rowNumber,
    int /*width*/,
    int /*height*/,
    bool rowIsSelected
) -> void
{
    auto alternateColour = getLookAndFeel()
                               .findColour(juce::ListBox::backgroundColourId)
                               .interpolatedWith(getLookAndFeel().findColour(juce::ListBox::textColourId), 0.03F);
    if (rowIsSelected) {
        g.fillAll(juce::Colours::lightblue);
    } else if ((rowNumber % 2) != 0) {
        g.fillAll(alternateColour);
    }
}

auto PorousAbsorberSimulationEditor::
    paintCell(juce::Graphics& g, int row, int columnId, int width, int height, bool /*rowIsSelected*/) -> void
{
    g.setColour(getLookAndFeel().findColour(juce::ListBox::textColourId));
    g.setFont(16.0F);

    if (columnId == 1) {
        auto const frequency = juce::String{_props[static_cast<size_t>(row)].first.numerical_value_in(si::hertz)};
        g.drawText(frequency, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    if (columnId == 2) {
        auto const absorptionFactor = juce::String{_props[static_cast<size_t>(row)].second.absorptionFactorNoAirGap};
        g.drawText(absorptionFactor, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    if (columnId == 3) {
        auto const absorptionFactor = juce::String{_props[static_cast<size_t>(row)].second.absorptionFactorWithAirGap};
        g.drawText(absorptionFactor, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    g.fillRect(width - 1, 0, 1, height);
}

auto PorousAbsorberSimulationEditor::updateSimulation() -> void
{
    _props.clear();

    auto const specs = PorousAbsorberSpecs{
        static_cast<double>(_absorberThickness),
        static_cast<double>(_absorberFlowResisitivity),
        static_cast<double>(_absorberAirGap),
    };

    auto const angle = static_cast<double>(_absorberAngleOfIncidence);

    auto const env = AtmosphericEnvironment{
        celciusToKelvin(_temperature),
        static_cast<double>(_pressure) * OneAtmosphere<double>,
    };

    auto const startFrequency = static_cast<double>(_plotStartFrequency) * si::hertz;
    auto const subDivisions   = static_cast<double>(_plotOctaveSubdivision);

    for (auto i{0}; i < static_cast<int>(_plotNumPoints); ++i) {
        auto const frequency = oactaveSubdivision(startFrequency, subDivisions, static_cast<double>(i));
        _props.emplace_back(frequency, propertiesOfAbsorber(specs, env, frequency, angle));
    }

    _table.updateContent();
    repaint();
}
}  // namespace ra
