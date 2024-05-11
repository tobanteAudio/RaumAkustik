#pragma once

#include <units/isq/si/thermodynamic_temperature.h>

namespace ra
{
using namespace units::isq;

template<typename T>
using Kelvin = si::thermodynamic_temperature<si::kelvin, T>;

[[nodiscard]] constexpr auto celciusToKelvin(double celcius) noexcept -> Kelvin<double>
{
    return Kelvin<double>{celcius + 273.15};
}

}  // namespace ra
