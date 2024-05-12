#pragma once

#include <ra/unit/frequency.hpp>
#include <ra/unit/pressure.hpp>
#include <ra/unit/speed.hpp>
#include <ra/unit/temperature.hpp>

#include <units/isq/si/density.h>

namespace ra {

using namespace units::isq;

[[nodiscard]] auto
densityOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept -> si::density<si::kilogram_per_metre_cub>;
[[nodiscard]] auto soundVelocity(Kelvin<double> temperature) noexcept -> MetrePerSecond<double>;
[[nodiscard]] auto impedanceOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept -> double;

}  // namespace ra
