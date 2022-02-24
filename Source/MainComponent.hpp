#pragma once

#include "FirstReflectionsView.hpp"
#include "LookAndFeel.hpp"
#include "PorousAbsorberSimulationView.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

struct CommandIDs
{
    enum Values
    {
        unknown,
        undo,
        redo,
        open,
        save,
    };
};

struct MenuBar final
    : juce::Component
    , juce::MenuBarModel
{
    MenuBar(juce::ApplicationCommandManager& commandManager) : _commandManager {commandManager}
    {
        setApplicationCommandManagerToWatch(&_commandManager);
        addAndMakeVisible(_menu);
    }

    ~MenuBar() override = default;

    auto resized() -> void override { _menu.setBounds(getLocalBounds()); }

    juce::StringArray getMenuBarNames() override { return {"File", "Edit"}; }

    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& /*menuName*/) override
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

    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override { }

private:
    juce::ApplicationCommandManager& _commandManager;
    juce::MenuBarComponent _menu {this};
};

struct MainComponent final
    : juce::Component
    , juce::ApplicationCommandTarget
{
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    auto getNextCommandTarget() -> juce::ApplicationCommandTarget*;
    auto getAllCommands(juce::Array<juce::CommandID>& commands) -> void;
    auto getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) -> void;
    auto perform(juce::ApplicationCommandTarget::InvocationInfo const& info) -> bool;

private:
    auto saveSession() -> void;
    auto loadSession() -> void;
    auto reloadUI() -> void;

    juce::ApplicationCommandManager _commandManager {};
    juce::UndoManager _undoManager;
    juce::ValueTree _valueTree {"RaumAkustik"};

    mc::LookAndFeel _lnf {};

    MenuBar _menuBar {_commandManager};

    juce::TabbedComponent _tabs {juce::TabbedButtonBar::TabsAtTop};
    std::unique_ptr<mc::FirstReflectionsView> _firstReflectionsView {};
    std::unique_ptr<mc::PorousAbsorberSimulationView> _absorberSimulationView {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
