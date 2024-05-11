#pragma once

#include <ra/unit/density.hpp>
#include <ra/unit/frequency.hpp>
#include <ra/unit/pressure.hpp>
#include <ra/unit/speed.hpp>
#include <ra/unit/temperature.hpp>

namespace ra {

[[nodiscard]] auto
densityOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept -> KilogramPerCubicMetre<double>;
[[nodiscard]] auto soundVelocity(Kelvin<double> temperature) noexcept -> MetrePerSecond<double>;
[[nodiscard]] auto impedanceOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept -> double;

}  // namespace ra
