#include "GlideSweep.hpp"

#include <ra/generator/SineOscillator.hpp>

namespace ra {

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
auto generate(GlideSweep const& spec) -> std::vector<float>
{
    auto scaleFrequency = [curve = spec.curve](auto f1, auto f2, auto t) {
        if (curve == GlideSweep::Curve::Logarithmic) {
            return std::pow(f1 * (f2 / f1), t);
        }
        return std::lerp(f1, f2, t);
    };

    auto const fs         = spec.sampleRate;
    auto const f1         = spec.from.numerical_value_in(si::hertz);
    auto const f2         = spec.to.numerical_value_in(si::hertz);
    auto const numSamples = static_cast<size_t>((spec.duration * fs).numerical_value_in(one));

    auto buffer = std::vector<float>(numSamples, 0.0F);
    auto osc    = SineOscillator<double>{fs.numerical_value_in(si::hertz)};

    for (auto i{0UL}; i < numSamples; ++i) {
        auto const t         = static_cast<double>(i) / static_cast<double>(numSamples - 1);
        auto const frequency = scaleFrequency(f1, f2, t);
        buffer[i]            = static_cast<float>(osc(frequency));
    }

    return buffer;
}

}  // namespace ra
