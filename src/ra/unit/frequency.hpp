#pragma once

#include <algorithm>

#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

namespace ra {

using namespace mp_units;

[[nodiscard]] auto oactaveSubdivision(quantity<isq::frequency[si::hertz]> start, double numSubdivisions, double index)
    -> quantity<isq::frequency[si::hertz]>;

}  // namespace ra
