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

}  // namespace ra
