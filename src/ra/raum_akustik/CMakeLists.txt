project(RaumAkustik VERSION 0.1.0)

option(RA_BUILD_ASIO "Build with ASIO driver support. Needs to be installed manually" OFF)

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

juce_add_gui_app(RaumAkustik
    PRODUCT_NAME "Raum Akustik"
    BUNDLE_ID    "com.tobanteAudio.raum-akustic"
    ICON_BIG     "resources/png/raum_akustik_logo_black.png"
)

target_sources(RaumAkustik
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

        "tool/latency_tester.cpp"
        "tool/latency_tester.hpp"
        "tool/measurement_recorder.cpp"
        "tool/measurement_recorder.hpp"
        "tool/noise_generator.cpp"
        "tool/noise_generator.hpp"

        "widget/level_meter.cpp"
        "widget/level_meter.hpp"
        "widget/scrolling_waveform.cpp"
        "widget/scrolling_waveform.hpp"
        "widget/spectogram.cpp"
        "widget/spectogram.hpp"
)

target_compile_definitions(RaumAkustik
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
)

target_link_libraries(RaumAkustik
    PRIVATE
        juce::juce_audio_devices
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra

        ra::acoustics
        RaumAkustik_BinaryData

        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND ${RA_BUILD_ASIO})
    target_compile_definitions(RaumAkustik PRIVATE JUCE_ASIO=1)
    target_include_directories(RaumAkustik PRIVATE ${CMAKE_SOURCE_DIR}/3rd_party/asiosdk_2.3.3_2019-06-14/common)
endif()


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL "15.0.0" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "15.1")
        target_compile_definitions(RaumAkustik PRIVATE JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)
        target_link_options(RaumAkustik PRIVATE -Wl,-ld_classic)
    endif()
endif()