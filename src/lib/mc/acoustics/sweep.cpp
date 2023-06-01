#include "sweep.hpp"

#include <cmath>
#include <numbers>

namespace mc
{

namespace
{
struct SinOscillator
{
    explicit SinOscillator(double fs) : _fs{fs} {}

    [[nodiscard]] auto operator()(double frequency) noexcept -> double
    {
        auto const output     = std::sin(_phase * std::numbers::pi * 2.0);
        auto const deltaPhase = 1.0 / (_fs / frequency);
        _phase += deltaPhase;
        _phase -= std::floor(_phase);
        return output;
    }

private:
    double _fs{0};
    double _phase{0};
};
}  // namespace

// x[n] = A * sin(2 * π * f[n] * n / fs + φ)
//
// Where:
//     - x[n] is the synthesized signal at sample index n.
//     - A is the amplitude of the sine wave.
//     - f[n] is the instantaneous frequency of the sine wave at sample index n.
//     - φ is the initial phase of the sine wave.
//     - π is a mathematical constant, approximately equal to 3.14159.
//     - fs is the sample rate (number of samples per second).
//
// The instantaneous frequency at each sample index can be determined using the following equation:
//
// f[n] = f1 + (f2 - f1) * (n - n1) / (n2 - n1)
//
// Where:
//     - f1 is the initial frequency of the sweep.
//     - f2 is the final frequency of the sweep.
//     - n1 is the starting sample index of the sweep.
//     - n2 is the ending sample index of the sweep.
auto makeSineSweep(SineSweepSpec const& spec) -> std::vector<float>
{
    using Seconds = std::chrono::duration<double>;

    auto const fs         = spec.sampleRate;
    auto const f1         = static_cast<double>(spec.start);
    auto const f2         = static_cast<double>(spec.end);
    auto const numSamples = static_cast<size_t>(std::chrono::duration_cast<Seconds>(spec.length).count() * fs);

    auto buffer = std::vector<float>(numSamples, 0.0F);
    auto osc    = SinOscillator{fs};

    for (auto i{0UL}; i < numSamples; ++i)
    {
        auto const t         = static_cast<double>(i) / static_cast<double>(numSamples - 1);
        auto const freq      = std::pow(f1 * (f2 / f1), t);
        auto const frequency = std::lerp(f1, f2, t);
        buffer[i]            = static_cast<float>(osc(freq));
    }

    return buffer;
}

}  // namespace mc
