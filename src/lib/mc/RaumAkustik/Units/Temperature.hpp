#pragma once

#include <units/isq/si/thermodynamic_temperature.h>

namespace mc
{
using namespace units::isq;
using Kelvin = si::thermodynamic_temperature<si::kelvin, double>;
[[nodiscard]] constexpr auto celciusToKelvin(double celcius) noexcept -> Kelvin { return Kelvin {celcius + 273.15}; }
}  // namespace mc
