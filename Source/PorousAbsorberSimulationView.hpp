#pragma once

#include "mc/RaumAkustik/PorousAbsorber.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

namespace mc
{
struct PorousAbsorberSimulationView final
    : juce::Component
    , juce::TableListBoxModel
    , juce::Value::Listener
{
    PorousAbsorberSimulationView();
    ~PorousAbsorberSimulationView() override = default;

    auto paint(juce::Graphics&) -> void override;
    auto resized() -> void override;

private:
    auto valueChanged(juce::Value& /*value*/) -> void override;

    auto getNumRows() -> int override;
    auto paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) -> void override;

    auto paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
        -> void override;

    juce::TableListBox _table {"Table", this};
    juce::PropertyPanel _absorberSpecs {};

    juce::Value _absorberThickness {};
    juce::Value _absorberFlowResisitivity {};
    juce::Value _absorberAngleOfIncidence {};

    std::vector<std::pair<double, PorousAbsorberProperties>> _props;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PorousAbsorberSimulationView)
};
}  // namespace mc