#pragma once

#include <units/isq/si/frequency.h>

namespace mc
{
using namespace units::isq;
using Hertz = si::frequency<si::hertz, double>;

[[nodiscard]] auto oactaveSubdivision(Hertz startFrequency, double numSubdivisions, double index) -> Hertz;
}  // namespace mc