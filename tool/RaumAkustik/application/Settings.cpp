#include "Settings.hpp"

#include "look/LookAndFeel.hpp"

#include "BinaryData.hpp"

namespace ra {

namespace {

[[nodiscard]] auto getIcon(char const* name) -> std::unique_ptr<juce::Drawable>
{
    auto size        = 0;
    auto const* data = mcbd::getNamedResource(name, size);
    jassert(data != nullptr);

    auto svg = juce::Drawable::createFromImageData(data, static_cast<size_t>(size));
    jassert(svg != nullptr);
    return svg;
}

}  // namespace

SettingsWindow::SettingsWindow(juce::ApplicationCommandManager& commandManager) : _commandManager{commandManager}
{
    auto speaker  = getIcon("speaker_svg");
    auto shortcut = getIcon("save_svg");
    addSettingsPage("Audio I/O", speaker.get(), speaker.get(), speaker.get());
    addSettingsPage("Look", shortcut.get(), shortcut.get(), shortcut.get());
    addSettingsPage("Shortcut", shortcut.get(), shortcut.get(), shortcut.get());

    _commandManager.getKeyMappings()->addChangeListener(this);
}

SettingsWindow::~SettingsWindow() { _commandManager.getKeyMappings()->removeChangeListener(this); }

auto SettingsWindow::createComponentForPage(juce::String const& pageName) -> juce::Component*
{
    if (pageName == "Audio I/O") {
        auto foo = _tree.getPropertyAsValue("foo", nullptr);
        auto bar = _tree.getPropertyAsValue("bar", nullptr);

        auto panel = std::make_unique<juce::PropertyPanel>();
        panel->addProperties(juce::Array<juce::PropertyComponent*>{
            std::make_unique<juce::SliderPropertyComponent>(foo, "Foo", 0.0, 1.0, 0.0).release(),
            std::make_unique<juce::TextPropertyComponent>(bar, "Bar", 32, false).release(),
        });
        return panel.release();
    }

    if (pageName == "Look") {
        auto panel = std::make_unique<juce::PropertyPanel>();
        panel->addProperties(juce::Array<juce::PropertyComponent*>{});
        return panel.release();
    }

    if (pageName == "Shortcut") {
        auto& mappings = *_commandManager.getKeyMappings();
        return std::make_unique<juce::KeyMappingEditorComponent>(mappings, true).release();
    }

    jassertfalse;
    return nullptr;
}

auto SettingsWindow::changeListenerCallback(juce::ChangeBroadcaster* source) -> void
{
    if (auto* mapping = _commandManager.getKeyMappings(); mapping == source) {
        // auto xml = mapping->createXml(true);
        // getApplicationSettings().setKeyMapping(*xml);
    }
}

}  // namespace ra