#include "frequency.hpp"

#define _USE_MATH_DEFINES  // NOLINT

#include <cmath>
#include <numbers>

namespace ra {

auto oactaveSubdivision(si::frequency<si::hertz> start, double numSubdivisions, double index)
    -> si::frequency<si::hertz>
{
    return si::frequency<si::hertz>{std::pow(2.0, std::log2(start.number()) + index / numSubdivisions)};
}

auto toAngularVelocity(si::frequency<si::hertz> hertz) -> si::angular_velocity<si::radian_per_second>
{
    static constexpr auto twoPi = std::numbers::pi * 2.0;
    return si::angular_velocity<si::radian_per_second>{twoPi * hertz.number()};
}
}  // namespace ra
