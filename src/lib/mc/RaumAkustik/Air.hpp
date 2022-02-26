#pragma once

#include "mc/RaumAkustik/Units.hpp"

namespace mc
{

[[nodiscard]] auto densityOfAir(Kelvin temperature, Pascal pressure) noexcept -> KilogramPerCubicMetre;
[[nodiscard]] auto soundVelocity(Kelvin temperature) noexcept -> MetrePerSecond;
[[nodiscard]] auto impedanceOfAir(Kelvin temperature, Pascal pressure) noexcept -> double;

}  // namespace mc
