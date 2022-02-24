#include "MenuBar.hpp"

#include "BinaryData.hpp"
#include "CommandIDs.hpp"

namespace mc
{
MenuBar::MenuBar(juce::ApplicationCommandManager& commandManager) : _commandManager {commandManager}
{
    setApplicationCommandManagerToWatch(&_commandManager);
    addAndMakeVisible(_menu);
}

auto MenuBar::resized() -> void { _menu.setBounds(getLocalBounds()); }

auto MenuBar::getMenuBarNames() -> juce::StringArray { return {"File", "Edit"}; }

auto MenuBar::getMenuForIndex(int menuIndex, const juce::String& /*menuName*/) -> juce::PopupMenu
{
    juce::PopupMenu menu;

    if (menuIndex == 0)
    {
        auto openIcon = juce::Drawable::createFromImageData(mcbd::folder_open_svg, mcbd::folder_open_svgSize);
        auto saveIcon = juce::Drawable::createFromImageData(mcbd::save_svg, mcbd::save_svgSize);
        menu.addCommandItem(&_commandManager, CommandIDs::open, {}, std::move(openIcon));
        menu.addCommandItem(&_commandManager, CommandIDs::save, {}, std::move(saveIcon));
    }
    else if (menuIndex == 1)
    {
        auto undoIcon = juce::Drawable::createFromImageData(mcbd::undo_svg, mcbd::undo_svgSize);
        auto redoIcon = juce::Drawable::createFromImageData(mcbd::redo_svg, mcbd::redo_svgSize);
        menu.addCommandItem(&_commandManager, CommandIDs::undo, {}, std::move(undoIcon));
        menu.addCommandItem(&_commandManager, CommandIDs::redo, {}, std::move(redoIcon));
    }
    // else if (menuIndex == 2)
    // {
    //     menu.addCommandItem (&_commandManager, CommandIDs::innerColourRed);
    //     menu.addCommandItem (&_commandManager, CommandIDs::innerColourGreen);
    //     menu.addCommandItem (&_commandManager, CommandIDs::innerColourBlue);
    // }

    return menu;
}

auto MenuBar::menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) -> void { }

}  // namespace mc