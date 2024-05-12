#pragma once

#include <units/isq/si/angular_velocity.h>
#include <units/isq/si/frequency.h>

namespace ra {
using namespace units::isq;

[[nodiscard]] auto
oactaveSubdivision(si::frequency<si::hertz> start, double numSubdivisions, double index) -> si::frequency<si::hertz>;
[[nodiscard]] auto toAngularVelocity(si::frequency<si::hertz> hertz) -> si::angular_velocity<si::radian_per_second>;

}  // namespace ra
