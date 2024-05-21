#pragma once

#include <algorithm>

#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

#include <chrono>
#include <vector>

namespace ra {

using namespace mp_units;

enum struct SineSweepCurve
{
    Linear,
    Logarithmic,
};

struct SineSweep
{
    quantity<isq::frequency[si::hertz]> from{20.0 * si::hertz};
    quantity<isq::frequency[si::hertz]> to{20'000.0 * si::hertz};
    SineSweepCurve curve{SineSweepCurve::Linear};
    std::chrono::milliseconds duration{1'000};
    double sampleRate{44'100.0};
};

[[nodiscard]] auto generate(SineSweep const& spec) -> std::vector<float>;

}  // namespace ra
