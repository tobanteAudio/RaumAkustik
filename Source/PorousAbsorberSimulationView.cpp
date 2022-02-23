#include "PorousAbsorberSimulationView.hpp"

#include "mc/RaumAkustik/Utility.hpp"

namespace mc
{

PorousAbsorberSimulationView::PorousAbsorberSimulationView()
{
    addAndMakeVisible(_table);
    addAndMakeVisible(_absorberSpecs);

    _table.getHeader().addColumn("Frequency", 1, 100, 100);
    _table.getHeader().addColumn("Reflection", 2, 100, 100);
    _table.getHeader().addColumn("Absorption", 3, 100, 100);

    _absorberThickness.getValue()        = 100.0;
    _absorberFlowResisitivity.getValue() = 8000.0;
    _absorberAngleOfIncidence.getValue() = 0.0;

    _absorberSpecs.addSection(
        "Absorber Dimensions",
        juce::Array<juce::PropertyComponent*> {
            new juce::SliderPropertyComponent {_absorberThickness, "Thickness (mm)", 0.0, 1000.0, 1.0},
            new juce::SliderPropertyComponent {_absorberFlowResisitivity, "Flow Resisitivity", 0.0, 10'000.0, 1.0},
            new juce::SliderPropertyComponent {_absorberAngleOfIncidence, "Angle Of Incidence", 0.0, 90.0, 1.0},

        });

    _absorberThickness.addListener(this);
    _absorberFlowResisitivity.addListener(this);
    _absorberAngleOfIncidence.addListener(this);
}

auto PorousAbsorberSimulationView::paint(juce::Graphics& g) -> void { (void)g; }

auto PorousAbsorberSimulationView::resized() -> void
{
    auto area = getLocalBounds();
    _absorberSpecs.setBounds(area.removeFromTop(area.proportionOfHeight(0.3)));
    _table.setBounds(area);
}

auto PorousAbsorberSimulationView::valueChanged(juce::Value& /*value*/) -> void
{
    _props.clear();

    auto const specs = PorousAbsorberSpecs {
        static_cast<double>(_absorberThickness.getValue()),
        static_cast<double>(_absorberFlowResisitivity.getValue()),
    };

    auto const env   = AtmosphericEnvironment {20.0, 1.0};
    auto const angle = static_cast<double>(_absorberAngleOfIncidence.getValue());

    for (auto i {0U}; i < 20U; ++i)
    {
        auto const frequency = oactaveSubdivision(50.0, 6, i);
        _props.push_back(std::make_pair(frequency, propertiesOfAbsorber(specs, env, frequency, angle)));
    }

    _table.updateContent();
    repaint();
}

auto PorousAbsorberSimulationView::getNumRows() -> int { return static_cast<int>(_props.size()); }

auto PorousAbsorberSimulationView::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height,
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
                                             bool rowIsSelected) -> void
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