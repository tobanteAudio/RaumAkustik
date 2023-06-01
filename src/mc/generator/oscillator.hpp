#pragma once

#include <cmath>
#include <numbers>

namespace mc
{

template<typename Float>
struct SinOscillator
{
    explicit SinOscillator(Float fs) noexcept : _fs{fs} {}

    [[nodiscard]] auto operator()(Float frequency) noexcept -> Float
    {
        static constexpr auto const twoPi = static_cast<Float>(std::numbers::pi * 2.0);

        auto const output     = std::sin(_phase * twoPi);
        auto const deltaPhase = Float(1) / (_fs / frequency);
        _phase += deltaPhase;
        _phase -= std::floor(_phase);

        return output;
    }

private:
    Float _fs{0};
    Float _phase{0};
};

}  // namespace mc
