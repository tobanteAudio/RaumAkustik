#include "application.hpp"

#include <mc/raum_akustik/app/main_component.hpp>

namespace mc
{

namespace
{
class MainWindow : public juce::DocumentWindow
{
public:
    explicit MainWindow(juce::String const& name)
        : DocumentWindow(
            name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
            DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new MainComponent(), true);  // NOLINT(cppcoreguidelines-owning-memory)

#if JUCE_IOS || JUCE_ANDROID
        setFullScreen(true);
#else
        setResizable(true, true);
        centreWithSize(getWidth(), getHeight());
#endif

        setVisible(true);
    }

    void closeButtonPressed() override
    {
        // This is called when the user tries to close this window. Here, we'll
        // just ask the app to quit when this happens, but you can change this to
        // do whatever you need.
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }

    /* Note: Be careful if you override any DocumentWindow methods - the base
       class uses a lot of them, so by overriding you might break its
       functionality. It's best to do all your work in your content component
       instead, but if you really have to override any DocumentWindow methods,
       make sure your subclass also calls the superclass's method.
    */

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)  // NOLINT
};
}  // namespace

auto RaumAkustikApplication::getApplicationName() -> const juce::String { return JUCE_APPLICATION_NAME_STRING; }

auto RaumAkustikApplication::getApplicationVersion() -> const juce::String { return JUCE_APPLICATION_VERSION_STRING; }

auto RaumAkustikApplication::moreThanOneInstanceAllowed() -> bool { return true; }

auto RaumAkustikApplication::initialise(juce::String const& commandLine) -> void
{
    // This method is where you should put your application's initialisation
    // code..
    juce::ignoreUnused(commandLine);

    _mainWindow = std::make_unique<MainWindow>(getApplicationName());
}

auto RaumAkustikApplication::shutdown() -> void
{
    // Add your application's shutdown code here..

    _mainWindow = nullptr;  // (deletes our window)
}

auto RaumAkustikApplication::systemRequestedQuit() -> void
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app
    // to close.
    quit();
}

auto RaumAkustikApplication::anotherInstanceStarted(juce::String const& commandLine) -> void
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
    juce::ignoreUnused(commandLine);
}

auto raumAkusticApplication() -> RaumAkustikApplication&
{
    auto* app            = juce::JUCEApplication::getInstance();
    auto* raumAkustikApp = dynamic_cast<RaumAkustikApplication*>(app);
    jassert(raumAkustikApp != nullptr);
    return *raumAkustikApp;
}

}  // namespace mc
