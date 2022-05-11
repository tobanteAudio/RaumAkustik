#include "Frequency.hpp"

#define _USE_MATH_DEFINES  // NOLINT

#include <cmath>
#include <numbers>

namespace mc
{

auto oactaveSubdivision(Hertz startFrequency, double numSubdivisions, double index) -> Hertz
{
    return Hertz{std::pow(2.0, std::log2(startFrequency.number()) + index / numSubdivisions)};
}

auto frequencyToAngularVelocity(Hertz hertz) -> RadianPerSecond
{
    static constexpr auto twoPi = std::numbers::pi * 2.0;
    return RadianPerSecond{twoPi * hertz.number()};
}
}  // namespace mc