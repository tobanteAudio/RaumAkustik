#pragma once

#include <mc/unit/density.hpp>
#include <mc/unit/frequency.hpp>
#include <mc/unit/pressure.hpp>
#include <mc/unit/speed.hpp>
#include <mc/unit/temperature.hpp>

namespace mc
{

[[nodiscard]] auto densityOfAir(Kelvin temperature, Pascal pressure) noexcept -> KilogramPerCubicMetre;
[[nodiscard]] auto soundVelocity(Kelvin temperature) noexcept -> MetrePerSecond;
[[nodiscard]] auto impedanceOfAir(Kelvin temperature, Pascal pressure) noexcept -> double;

}  // namespace mc
