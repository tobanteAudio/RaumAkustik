#include "MainComponent.hpp"

struct RaumAkustikApplication : juce::JUCEApplication
{
    RaumAkustikApplication() = default;

    auto getApplicationName() -> const juce::String override { return JUCE_APPLICATION_NAME_STRING; }
    auto getApplicationVersion() -> const juce::String override { return JUCE_APPLICATION_VERSION_STRING; }
    auto moreThanOneInstanceAllowed() -> bool override { return true; }

    void initialise(juce::String const& commandLine) override
    {
        // This method is where you should put your application's initialisation
        // code..
        juce::ignoreUnused(commandLine);

        _mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        _mainWindow = nullptr;  // (deletes our window)
    }

    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app
        // to close.
        quit();
    }

    void anotherInstanceStarted(juce::String const& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
        juce::ignoreUnused(commandLine);
    }

    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        explicit MainWindow(juce::String const& name)
            : DocumentWindow(
                name,
                juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
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
            JUCEApplication::getInstance()->systemRequestedQuit();
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

private:
    std::unique_ptr<MainWindow> _mainWindow;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(RaumAkustikApplication)  // NOLINT
