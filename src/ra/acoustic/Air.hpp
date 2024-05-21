#pragma once

#include <algorithm>

#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

namespace ra {

using namespace mp_units;

[[nodiscard]] auto densityOfAir(
    quantity<isq::thermodynamic_temperature[si::kelvin]> temperature,
    quantity<isq::pressure[si::pascal]> pressure
) noexcept -> quantity<isq::density[si::kilogram / cubic(si::metre)]>;
[[nodiscard]] auto soundVelocity(quantity<isq::thermodynamic_temperature[si::kelvin]> temperature
) noexcept -> quantity<isq::speed[si::metre / si::second]>;
[[nodiscard]] auto impedanceOfAir(
    quantity<isq::thermodynamic_temperature[si::kelvin]> temperature,
    quantity<isq::pressure[si::pascal]> pressure
) noexcept -> double;

}  // namespace ra
