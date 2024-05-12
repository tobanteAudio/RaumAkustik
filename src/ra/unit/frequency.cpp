#include "frequency.hpp"

#define _USE_MATH_DEFINES  // NOLINT

#include <cmath>
#include <numbers>

namespace ra {

auto oactaveSubdivision(Hertz<double> startFrequency, double numSubdivisions, double index) -> Hertz<double>
{
    return Hertz<double>{std::pow(2.0, std::log2(startFrequency.number()) + index / numSubdivisions)};
}

auto toAngularVelocity(Hertz<double> hertz) -> si::angular_velocity<si::radian_per_second>
{
    static constexpr auto twoPi = std::numbers::pi * 2.0;
    return si::angular_velocity<si::radian_per_second>{twoPi * hertz.number()};
}
}  // namespace ra
