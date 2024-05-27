#include "ProjectTreeViewRoot.hpp"

#include "look/LookAndFeel.hpp"

namespace ra {

auto ProjectTreeViewRoot::mightContainSubItems() -> bool { return true; }

auto ProjectTreeViewRoot::getUniqueName() const -> juce::String { return "project_root"; }

auto ProjectTreeViewRoot::itemOpennessChanged(bool isNowOpen) -> void
{
    if (isNowOpen) {
    } else {
        clearSubItems();
    }
}

auto ProjectTreeViewRoot::paintItem(juce::Graphics& g, int width, int height) -> void
{
    auto const bounds = juce::Rectangle{0, 0, width, height}.toFloat();
    g.setColour(getSchemeDefaultTextColour());
    g.drawText("Project", bounds, juce::Justification::centred);
}

auto ProjectTreeViewRoot::itemClicked(juce::MouseEvent const& event) -> void
{
    if (event.mods.isRightButtonDown()) {
        auto menu = juce::PopupMenu{};
        menu.addItem("Foo", [] {});
        menu.addItem("Bar", [] {});
        menu.addItem("Baz", [] {});

        auto options = juce::PopupMenu::Options().withTargetComponent(getOwnerView()).withMousePosition();
        menu.showMenuAsync(options);
    }
}

}  // namespace ra
