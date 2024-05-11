#include "frequency.hpp"

#define _USE_MATH_DEFINES  // NOLINT

#include <cmath>
#include <numbers>

namespace ra {

auto oactaveSubdivision(Hertz<double> startFrequency, double numSubdivisions, double index) -> Hertz<double>
{
    return Hertz<double>{std::pow(2.0, std::log2(startFrequency.number()) + index / numSubdivisions)};
}

auto frequencyToAngularVelocity(Hertz<double> hertz) -> RadianPerSecond<double>
{
    static constexpr auto twoPi = std::numbers::pi * 2.0;
    return RadianPerSecond<double>{twoPi * hertz.number()};
}
}  // namespace ra
