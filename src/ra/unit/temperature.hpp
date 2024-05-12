#pragma once

#include <units/isq/si/thermodynamic_temperature.h>

namespace ra {
using namespace units::isq;

[[nodiscard]] constexpr auto celciusToKelvin(double celcius) noexcept -> si::thermodynamic_temperature<si::kelvin>
{
    return si::thermodynamic_temperature<si::kelvin>{celcius + 273.15};
}

}  // namespace ra
