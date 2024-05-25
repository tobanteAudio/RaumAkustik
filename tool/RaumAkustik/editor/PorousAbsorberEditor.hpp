#pragma once

#include <ra/acoustic/absorber/PorousAbsorber.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {

struct PorousAbsorberEditor final
    : juce::Component
    , juce::TableListBoxModel
    , juce::ValueTree::Listener
{
    PorousAbsorberEditor(juce::ValueTree vt, juce::UndoManager* um);
    ~PorousAbsorberEditor() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

    [[nodiscard]] static auto createDefault(juce::ValueTree parent, juce::UndoManager* um) -> juce::ValueTree;

private:
    struct IDs
    {
        static constexpr auto const* type = "PorousAbsorber";

        static constexpr auto const* temperature = "temperature";
        static constexpr auto const* pressure    = "pressure";

        static constexpr auto const* absorberThickness        = "absorberThickness";
        static constexpr auto const* absorberFlowResisitivity = "absorberFlowResisitivity";
        static constexpr auto const* absorberAngleOfIncidence = "absorberAngleOfIncidence";
        static constexpr auto const* absorberAirGap           = "absorberAirGap";

        static constexpr auto const* plotNumPoints         = "plotNumPoints";
        static constexpr auto const* plotStartFrequency    = "plotStartFrequency";
        static constexpr auto const* plotOctaveSubdivision = "plotOctaveSubdivision";
    };

    auto getNumRows() -> int override;
    auto paintRowBackground(juce::Graphics& g, int row, int width, int height, bool isSelected) -> void override;
    auto paintCell(juce::Graphics& g, int row, int column, int width, int height, bool isSelected) -> void override;

    auto valueTreePropertyChanged(juce::ValueTree& tree, juce::Identifier const& property) -> void override;

    auto updateSimulation() -> void;

    juce::TableListBox _table{"Table", this};
    juce::PropertyPanel _absorberSpecs;
    juce::Rectangle<int> _plotArea;

    juce::UndoManager* _undoManager{nullptr};
    juce::ValueTree _valueTree;

    juce::CachedValue<double> _temperature{_valueTree, IDs::temperature, _undoManager};
    juce::CachedValue<double> _pressure{_valueTree, IDs::pressure, _undoManager};

    juce::CachedValue<double> _absorberThickness{_valueTree, IDs::absorberThickness, _undoManager};
    juce::CachedValue<double> _absorberFlowResisitivity{_valueTree, IDs::absorberFlowResisitivity, _undoManager};
    juce::CachedValue<double> _absorberAngleOfIncidence{_valueTree, IDs::absorberAngleOfIncidence, _undoManager};
    juce::CachedValue<double> _absorberAirGap{_valueTree, IDs::absorberAirGap, _undoManager};

    juce::CachedValue<double> _plotNumPoints{_valueTree, IDs::plotNumPoints, _undoManager};
    juce::CachedValue<double> _plotStartFrequency{_valueTree, IDs::plotStartFrequency, _undoManager};
    juce::CachedValue<double> _plotOctaveSubdivision{_valueTree, IDs::plotOctaveSubdivision, _undoManager};

    std::vector<std::pair<quantity<isq::frequency[si::hertz]>, PorousAbsorberProperties>> _props;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PorousAbsorberEditor)  // NOLINT
};
}  // namespace ra
