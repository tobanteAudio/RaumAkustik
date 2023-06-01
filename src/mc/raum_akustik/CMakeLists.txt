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
        "app/application.cpp"
        "app/application.hpp"
        "app/command_ids.hpp"
        "app/main_component.cpp"
        "app/main_component.hpp"
        "app/main.cpp"
        "app/menu_bar.cpp"
        "app/menu_bar.hpp"

        "look/look_and_feel.cpp"
        "look/look_and_feel.hpp"

        "tabs/absorber_simulation_tab.cpp"
        "tabs/absorber_simulation_tab.hpp"
        "tabs/audio_input_tab.cpp"
        "tabs/audio_input_tab.hpp"
        "tabs/first_reflections_tab.cpp"
        "tabs/first_reflections_tab.hpp"
        "tabs/generator_tab.cpp"
        "tabs/generator_tab.hpp"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_ASIO=1
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/3rd_party/asiosdk_2.3.3_2019-06-14/common
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_devices
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra

        mc::acoustics
        RaumAkustik_BinaryData

        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
