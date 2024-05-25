#pragma once

#include <juce_audio_formats/juce_audio_formats.h>

namespace ra {

inline auto writeToWavFile(juce::File const& file, std::vector<float> const& buffer, double fs, int bits = 24) -> bool
{
    if (file.existsAsFile()) {
        file.deleteFile();
    }
    auto stream = file.createOutputStream();
    auto wav    = juce::WavAudioFormat{};
    auto writer = std::unique_ptr<juce::AudioFormatWriter>{wav.createWriterFor(stream.get(), fs, 1, bits, {}, 0)};
    if (writer) {
        juce::ignoreUnused(stream.release());
    }
    auto channels = std::array<float const*, 1>{buffer.data()};
    return writer->writeFromFloatArrays(channels.data(), 1, static_cast<int>(buffer.size()));
}

}  // namespace ra
