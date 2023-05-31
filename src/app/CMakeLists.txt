project(RaumAkustik VERSION 0.1.0)

juce_add_binary_data(RaumAkustik_BinaryData
    HEADER_NAME
        "BinaryData.hpp"
    NAMESPACE
        mcbd
    SOURCES
        "resources/png/raum_akustik_logo_black.png"

        "resources/svg/add.svg"
        "resources/svg/arrow_back.svg"
        "resources/svg/arrow_forward.svg"
        "resources/svg/delete.svg"
        "resources/svg/edit.svg"
        "resources/svg/folder_open.svg"
        "resources/svg/fullscreen.svg"
        "resources/svg/info.svg"
        "resources/svg/insert_emoticon.svg"
        "resources/svg/raum_akustik_logo_black.svg"
        "resources/svg/redo.svg"
        "resources/svg/remove.svg"
        "resources/svg/save.svg"
        "resources/svg/settings.svg"
        "resources/svg/speaker.svg"
        "resources/svg/undo.svg"
)

juce_add_gui_app(${PROJECT_NAME}
    PRODUCT_NAME "Raum Akustik"
    BUNDLE_ID    "com.modern-circuits.raum-akustic"
    ICON_BIG     "resources/png/raum_akustik_logo_black.png"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        "AudioInputView.cpp"
        "AudioInputView.hpp"
        "CommandIDs.hpp"
        "FirstReflectionsView.cpp"
        "FirstReflectionsView.hpp"
        "LookAndFeel.cpp"
        "LookAndFeel.hpp"
        "Main.cpp"
        "MainComponent.cpp"
        "MainComponent.hpp"
        "MenuBar.cpp"
        "MenuBar.hpp"
        "PorousAbsorberSimulationView.cpp"
        "PorousAbsorberSimulationView.hpp"
        "RaumAkustikApplication.cpp"
        "RaumAkustikApplication.hpp"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_devices
        juce::juce_audio_utils
        mc::acoustics
        RaumAkustik_BinaryData
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
