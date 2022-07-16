#pragma once

#include <units/isq/si/angular_velocity.h>
#include <units/isq/si/frequency.h>

namespace mc
{
using namespace units::isq;
using Hertz           = si::frequency<si::hertz, double>;
using RadianPerSecond = si::angular_velocity<si::radian_per_second, double>;

[[nodiscard]] auto oactaveSubdivision(Hertz startFrequency, double numSubdivisions, double index) -> Hertz;
[[nodiscard]] auto frequencyToAngularVelocity(Hertz hertz) -> RadianPerSecond;

}  // namespace mc
