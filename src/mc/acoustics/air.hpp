#pragma once

#include <mc/unit/density.hpp>
#include <mc/unit/frequency.hpp>
#include <mc/unit/pressure.hpp>
#include <mc/unit/speed.hpp>
#include <mc/unit/temperature.hpp>

namespace ra
{

[[nodiscard]] auto densityOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept
    -> KilogramPerCubicMetre<double>;
[[nodiscard]] auto soundVelocity(Kelvin<double> temperature) noexcept -> MetrePerSecond<double>;
[[nodiscard]] auto impedanceOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept -> double;

}  // namespace ra
