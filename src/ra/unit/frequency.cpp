#include "frequency.hpp"

#define _USE_MATH_DEFINES  // NOLINT

#include <cmath>
#include <numbers>

namespace ra {

auto oactaveSubdivision(quantity<isq::frequency[si::hertz]> start, double numSubdivisions, double index)
    -> quantity<isq::frequency[si::hertz]>
{
    return std::pow(2.0, std::log2(start.numerical_value_in(si::hertz)) + index / numSubdivisions) * si::hertz;
}

auto toAngularVelocity(quantity<isq::frequency[si::hertz]> hertz
) -> quantity<isq::angular_frequency[si::radian / si::second]>
{
    static constexpr auto twoPi = std::numbers::pi * 2.0;
    return twoPi * hertz.numerical_value_in(si::hertz) * si::radian / si::second;
}
}  // namespace ra
