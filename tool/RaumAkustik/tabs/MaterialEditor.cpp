#include "MaterialEditor.hpp"

#include "look/Colors.hpp"

namespace ra {

namespace {

constexpr auto Bands = std::array<char const*, 22>{
    "63",  "80",   "100",  "125",  "160",  "200",  "250",  "315",  "400",  "500",  "630",
    "800", "1000", "1250", "1600", "2000", "2500", "3150", "4000", "5000", "6300", "8000",
};

[[nodiscard]] auto loadMaterials(juce::File const& path) -> std::vector<AcousticMaterial>
{

    auto input  = juce::FileInputStream{path};
    auto result = std::vector<AcousticMaterial>{};

    auto jsonFile = juce::JSON::parse(input);
    auto* list    = jsonFile.getArray();
    jassert(list != nullptr);

    for (auto const& m : *list) {
        auto* obj     = m.getDynamicObject();
        auto material = AcousticMaterial{
            .name           = obj->getProperty("description").toString(),
            .type           = obj->getProperty("type").toString(),
            .flowResistance = obj->getProperty("flow resistance").toString(),
            .absorption     = {},
        };

        for (auto const* band : Bands) {
            auto b = obj->getProperty(band);
            if (b.isDouble()) {
                material.absorption[band] = static_cast<double>(b);
            }
        }

        result.push_back(std::move(material));
    }

    return result;
}
}  // namespace

MaterialEditor::MaterialEditor()
    : _materials{loadMaterials(juce::File("/Users/tobante/Developer/neo-sonar/neo-acoustic/material.json"))}
    , _sortedMaterials{_materials}
{
    _table.getHeader().addColumn("Material", NameColumnIndex, 150, 150);
    _table.getHeader().addColumn("Type", TypeColumnIndex, 150, 150);
    _table.getHeader().addColumn("Flow Resistance", FlowResistanceColumnIndex, 150, 150);

    for (auto i{0}; i < static_cast<int>(Bands.size()); ++i) {
        _table.getHeader().addColumn(Bands[static_cast<size_t>(i)], FirstBandColumnIndex + i, 50, 95);
    }

    _table.updateContent();

    addAndMakeVisible(_table);
}

auto MaterialEditor::paint(juce::Graphics& g) -> void
{
    g.fillAll(juce::Colours::white);

    auto selected = _table.getSelectedRows();
    if (selected.isEmpty()) {
        return;
    }

    auto const idx                             = static_cast<size_t>(selected[0]);
    auto const& [name, type, flow, absorption] = _sortedMaterials.at(idx);

    auto const plot = _plot.reduced(8.0F);
    g.setColour(juce::Colours::black);
    g.drawText(name, plot, juce::Justification::centredTop);

    auto const deltaX = plot.getWidth() / static_cast<float>(Bands.size());
    for (auto i{0U}; i < Bands.size(); ++i) {
        if (auto const band = juce::String(Bands[i]); absorption.contains(band)) {
            auto const value  = absorption.at(band);
            auto const x      = plot.getX() + deltaX * static_cast<float>(i);
            auto const height = juce::jmap(static_cast<float>(value), 0.0F, 1.0F, 0.0F, plot.getHeight());
            auto const bar    = juce::Rectangle{0.0F, 0.0F, deltaX * 0.33F, height};
            g.fillRect(bar.withX(x).withBottomY(plot.getBottom()));
        }
    }
}

auto MaterialEditor::resized() -> void
{
    auto area = getLocalBounds();
    _plot     = area.removeFromTop(area.proportionOfHeight(0.5)).toFloat();
    _table.setBounds(area);
}

auto MaterialEditor::getNumRows() -> int { return static_cast<int>(_sortedMaterials.size()); }

auto MaterialEditor::paintRowBackground(juce::Graphics& g, int /*row*/, int width, int height, bool isSelected) -> void
{
    auto const color = isSelected ? juce::Colours::black : juce::Colours::white;
    g.setColour(color);
    g.fillRect(juce::Rectangle{0, 0, width, height});
}

auto MaterialEditor::paintCell(juce::Graphics& g, int row, int col, int width, int height, bool isSelected) -> void
{
    if (std::cmp_greater_equal(row, _sortedMaterials.size())) {
        return;
    }

    auto const& material = _sortedMaterials[static_cast<size_t>(row)];
    auto const area      = juce::Rectangle{0, 0, width, height};
    auto const color     = isSelected ? juce::Colours::white : juce::Colours::black;
    g.setColour(color);

    if (col == NameColumnIndex) {
        g.drawText(material.name, area, juce::Justification::left);
    }

    if (col == TypeColumnIndex) {
        g.drawText(material.type, area, juce::Justification::left);
    }

    if (col == FlowResistanceColumnIndex) {
        g.drawText(material.flowResistance, area, juce::Justification::left);
    }

    for (auto i{0U}; i < Bands.size(); ++i) {
        if (std::cmp_equal(col, unsigned(FirstBandColumnIndex) + i)) {
            auto const band = juce::String(Bands[i]);
            if (material.absorption.contains(band)) {
                auto const value = material.absorption.at(band);
                auto const text  = juce::String(value, 2);
                g.drawText(text, area, juce::Justification::right);
            }
        }
    }
}

auto MaterialEditor::selectedRowsChanged(int lastRowSelected) -> void
{
    if (std::cmp_greater_equal(lastRowSelected, _sortedMaterials.size())) {
        return;
    }

    repaint();
}

auto MaterialEditor::sortOrderChanged(int newSortColumnId, bool isForwards) -> void
{
    _sortedMaterials = _materials;

    auto const sortByText = [&](auto member) {
        if (isForwards) {
            std::ranges::sort(_sortedMaterials, std::less(), member);
        } else {
            std::ranges::sort(_sortedMaterials, std::greater(), member);
        }
    };

    if (newSortColumnId == NameColumnIndex) {
        sortByText(&AcousticMaterial::name);
        return;
    }

    if (newSortColumnId == TypeColumnIndex) {
        sortByText(&AcousticMaterial::type);
        return;
    }

    if (newSortColumnId == FlowResistanceColumnIndex) {
        sortByText(&AcousticMaterial::flowResistance);
        return;
    }

    auto const bandIndex = newSortColumnId - FirstBandColumnIndex;
    if (bandIndex >= 0 and bandIndex < static_cast<int>(Bands.size())) {
        auto const band = juce::String{Bands[size_t(bandIndex)]};

        std::ranges::sort(_sortedMaterials, [&](auto const& lhs, auto const& rhs) {
            auto l = std::optional<double>{};
            if (lhs.absorption.contains(band)) {
                l = lhs.absorption.at(band);
            }

            auto r = std::optional<double>{};
            if (rhs.absorption.contains(band)) {
                r = rhs.absorption.at(band);
            }

            if (isForwards) {
                return l < r;
            }

            return l > r;
        });
    }
}

}  // namespace ra
