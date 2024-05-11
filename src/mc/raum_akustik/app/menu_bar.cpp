#include "menu_bar.hpp"

#include <mc/raum_akustik/app/command_ids.hpp>

#include "BinaryData.hpp"

namespace ra
{
MenuBar::MenuBar(juce::ApplicationCommandManager& commandManager) : _commandManager{commandManager}
{
    setApplicationCommandManagerToWatch(&_commandManager);
    addAndMakeVisible(_menu);
}

auto MenuBar::resized() -> void { _menu.setBounds(getLocalBounds()); }

auto MenuBar::getMenuBarNames() -> juce::StringArray
{
    return {
        "File",
        "Edit",
        "View",
        "Help",
    };
}

auto MenuBar::getMenuForIndex(int menuIndex, juce::String const& /*menuName*/) -> juce::PopupMenu
{

    if (menuIndex == 0)
    {
        auto menu     = juce::PopupMenu{};
        auto openIcon = juce::Drawable::createFromImageData(mcbd::folder_open_svg, mcbd::folder_open_svgSize);
        auto saveIcon = juce::Drawable::createFromImageData(mcbd::save_svg, mcbd::save_svgSize);
        menu.addCommandItem(&_commandManager, CommandIDs::open, {}, openIcon->createCopy());
        menu.addCommandItem(&_commandManager, CommandIDs::save, {}, saveIcon->createCopy());
        menu.addCommandItem(&_commandManager, CommandIDs::saveAs, {}, saveIcon->createCopy());
        return menu;
    }

    if (menuIndex == 1)
    {
        auto menu         = juce::PopupMenu{};
        auto undoIcon     = juce::Drawable::createFromImageData(mcbd::undo_svg, mcbd::undo_svgSize);
        auto redoIcon     = juce::Drawable::createFromImageData(mcbd::redo_svg, mcbd::redo_svgSize);
        auto settingsIcon = juce::Drawable::createFromImageData(mcbd::settings_svg, mcbd::settings_svgSize);
        menu.addCommandItem(&_commandManager, CommandIDs::undo, {}, undoIcon->createCopy());
        menu.addCommandItem(&_commandManager, CommandIDs::redo, {}, redoIcon->createCopy());
        menu.addSeparator();
        menu.addCommandItem(&_commandManager, CommandIDs::settings, {}, settingsIcon->createCopy());
        return menu;
    }

    if (menuIndex == 2)
    {
        auto menu           = juce::PopupMenu{};
        auto backIcon       = juce::Drawable::createFromImageData(mcbd::arrow_back_svg, mcbd::arrow_back_svgSize);
        auto forwardIcon    = juce::Drawable::createFromImageData(mcbd::arrow_forward_svg, mcbd::arrow_forward_svgSize);
        auto fullscreenIcon = juce::Drawable::createFromImageData(mcbd::fullscreen_svg, mcbd::fullscreen_svgSize);
        menu.addCommandItem(&_commandManager, CommandIDs::gotoTabLeft, {}, backIcon->createCopy());
        menu.addCommandItem(&_commandManager, CommandIDs::gotoTabRight, {}, forwardIcon->createCopy());
        menu.addSeparator();
        menu.addCommandItem(&_commandManager, CommandIDs::fullscreen, {}, fullscreenIcon->createCopy());
        return menu;
    }

    if (menuIndex == 3)
    {
        auto menu     = juce::PopupMenu{};
        auto infoIcon = juce::Drawable::createFromImageData(mcbd::info_svg, mcbd::info_svgSize);
        menu.addCommandItem(&_commandManager, CommandIDs::about, {}, infoIcon->createCopy());
        return menu;
    }

    jassertfalse;
    return {};
}

auto MenuBar::menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) -> void {}

}  // namespace ra
