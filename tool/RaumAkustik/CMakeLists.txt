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
        "application/Application.cpp"
        "application/Application.hpp"
        "application/CommandIDs.hpp"
        "application/MainComponent.cpp"
        "application/MainComponent.hpp"
        "application/Main.cpp"
        "application/MenuBar.cpp"
        "application/MenuBar.hpp"
        "application/Settings.hpp"
        "application/Settings.cpp"

        "component/LevelMeter.cpp"
        "component/LevelMeter.hpp"
        "component/ScrollingWaveform.cpp"
        "component/ScrollingWaveform.hpp"
        "component/Spectogram.cpp"
        "component/Spectogram.hpp"

        "look/ColorMap.hpp"
        "look/ColorRGB.hpp"
        "look/LookAndFeel.cpp"
        "look/LookAndFeel.hpp"

        "project/ProjectTreeViewRoot.cpp"
        "project/ProjectTreeViewRoot.hpp"

        "editor/PorousAbsorberEditor.cpp"
        "editor/PorousAbsorberEditor.hpp"
        "editor/AudioInterfaceEditor.cpp"
        "editor/AudioInterfaceEditor.hpp"
        "editor/MaterialEditor.cpp"
        "editor/MaterialEditor.hpp"
        "editor/RoomEditor.cpp"
        "editor/RoomEditor.hpp"
        "editor/StochasticRaytracingEditor.cpp"
        "editor/StochasticRaytracingEditor.hpp"
        "editor/ToneGeneratorEditor.cpp"
        "editor/ToneGeneratorEditor.hpp"
        "editor/WaveEquation2DEditor.cpp"
        "editor/WaveEquation2DEditor.hpp"

        "tool/LatencyTester.cpp"
        "tool/LatencyTester.hpp"
        "tool/MeasurementRecorder.cpp"
        "tool/MeasurementRecorder.hpp"
        "tool/NoiseGenerator.cpp"
        "tool/NoiseGenerator.hpp"
        "tool/PropertyComponent.hpp"

        "utility/ValueTree.hpp"
)

target_compile_definitions(RaumAkustik
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
)

target_link_libraries(RaumAkustik
    PRIVATE
        juce::juce_audio_devices
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
        neo::neo_gui_extra

        ra::acoustics
        Boost::headers
        concurrentqueue::concurrentqueue

        RaumAkustik_BinaryData

        juce::juce_recommended_config_flags
        # juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(RaumAkustik PUBLIC ${PROJECT_SOURCE_DIR})

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
