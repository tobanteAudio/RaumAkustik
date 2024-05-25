#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ra {

struct ProjectTreeViewRoot final : juce::TreeViewItem
{
    ProjectTreeViewRoot()           = default;
    ~ProjectTreeViewRoot() override = default;

    auto mightContainSubItems() -> bool override;
    auto getUniqueName() const -> juce::String override;
    auto itemOpennessChanged(bool isNowOpen) -> void override;
    auto paintItem(juce::Graphics& g, int width, int height) -> void override;
    auto itemClicked(juce::MouseEvent const& event) -> void override;

private:
};

}  // namespace ra
