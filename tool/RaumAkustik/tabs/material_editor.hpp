#pragma once

#include "tool/measurement_recorder.hpp"

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {

struct AcousticMaterial
{
    juce::String name;
    juce::String type;
    juce::String flowResistance;
    std::map<juce::String, double, std::less<>> absorption;
};

struct MaterialEditor final
    : juce::Component
    , juce::TableListBoxModel
{
    MaterialEditor();
    ~MaterialEditor() override = default;

    auto paint(juce::Graphics& g) -> void override;
    auto resized() -> void override;

    auto getNumRows() -> int override;
    auto paintRowBackground(juce::Graphics& g, int row, int width, int height, bool isSelected) -> void override;
    auto paintCell(juce::Graphics& g, int row, int columnId, int width, int height, bool isSelected) -> void override;
    auto selectedRowsChanged(int lastRowSelected) -> void override;
    auto sortOrderChanged(int newSortColumnId, bool isForwards) -> void override;

private:
    enum Columns
    {
        NameColumnIndex = 1,
        TypeColumnIndex,
        FlowResistanceColumnIndex,
        FirstBandColumnIndex,
    };

    std::vector<AcousticMaterial> _materials;
    std::vector<AcousticMaterial> _sortedMaterials;

    juce::TableListBox _table{"Table", this};
    juce::Rectangle<float> _plot;
};

}  // namespace ra
