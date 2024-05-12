#pragma once

#include <units/isq/si/angular_velocity.h>
#include <units/isq/si/frequency.h>

namespace ra {
using namespace units::isq;

template<typename T>
using Hertz = si::frequency<si::hertz, T>;

[[nodiscard]] auto
oactaveSubdivision(Hertz<double> startFrequency, double numSubdivisions, double index) -> Hertz<double>;
[[nodiscard]] auto toAngularVelocity(Hertz<double> hertz) -> si::angular_velocity<si::radian_per_second>;

}  // namespace ra
