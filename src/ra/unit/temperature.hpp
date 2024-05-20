#pragma once

#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

namespace ra {
using namespace mp_units;

[[nodiscard]] constexpr auto celciusToKelvin(double celcius
) noexcept -> quantity<isq::thermodynamic_temperature[si::kelvin]>
{
    return (celcius + 273.15) * si::kelvin;
}

}  // namespace ra
