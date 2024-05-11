#pragma once

#include <ra/acoustics/absorber/porous_absorber.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {
struct PorousAbsorberSimulationView final
    : juce::Component
    , juce::TableListBoxModel
    , juce::ValueTree::Listener
{
    PorousAbsorberSimulationView(juce::ValueTree vt, juce::UndoManager* um);
    ~PorousAbsorberSimulationView() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

private:
    auto valueTreePropertyChanged(juce::ValueTree& tree, juce::Identifier const& property) -> void override;

    auto getNumRows() -> int override;
    auto
    paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) -> void override;

    auto paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
        -> void override;

    auto updateSimulation() -> void;

    juce::TableListBox _table{"Table", this};
    juce::PropertyPanel _absorberSpecs;
    juce::Rectangle<int> _plotArea;

    juce::UndoManager* _undoManager{nullptr};
    juce::ValueTree _valueTree;

    juce::CachedValue<double> _temperature{_valueTree, "temperature", _undoManager};
    juce::CachedValue<double> _pressure{_valueTree, "pressure", _undoManager};

    juce::CachedValue<double> _absorberThickness{_valueTree, "absorberThickness", _undoManager};
    juce::CachedValue<double> _absorberFlowResisitivity{_valueTree, "absorberFlowResisitivity", _undoManager};
    juce::CachedValue<double> _absorberAngleOfIncidence{_valueTree, "absorberAngleOfIncidence", _undoManager};
    juce::CachedValue<double> _absorberAirGap{_valueTree, "absorberAirGap", _undoManager};

    juce::CachedValue<double> _plotNumPoints{_valueTree, "plotNumPoints", _undoManager};
    juce::CachedValue<double> _plotStartFrequency{_valueTree, "plotStartFrequency", _undoManager};
    juce::CachedValue<double> _plotOctaveSubdivision{_valueTree, "plotOctaveSubdivision", _undoManager};

    std::vector<std::pair<Hertz<double>, PorousAbsorberProperties>> _props;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PorousAbsorberSimulationView)  // NOLINT
};
}  // namespace ra
