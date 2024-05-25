#pragma once

#include <juce_graphics/juce_graphics.h>

namespace ra {

struct ColorRGB
{
    float r;
    float g;
    float b;

    [[nodiscard]] operator juce::Colour() const noexcept  // NOLINT
    {
        return juce::Colour::fromFloatRGBA(r, g, b, 1.0F);
    }
};

}  // namespace ra
