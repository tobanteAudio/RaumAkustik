#pragma once

#include <ra/unit/frequency.hpp>

#include <chrono>
#include <vector>

namespace ra
{

enum struct SineSweepCurve
{
    Linear,
    Logarithmic,
};

struct SineSweep
{
    si::frequency<si::hertz> from{20.0};
    si::frequency<si::hertz> to{20'000.0};
    SineSweepCurve curve{SineSweepCurve::Linear};
    std::chrono::milliseconds duration{1'000};
    double sampleRate{44'100.0};
};

[[nodiscard]] auto generate(SineSweep const& spec) -> std::vector<float>;

}  // namespace ra