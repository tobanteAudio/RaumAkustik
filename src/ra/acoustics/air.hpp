#pragma once

#include <ra/unit/frequency.hpp>
#include <ra/unit/pressure.hpp>
#include <ra/unit/temperature.hpp>

#include <units/isq/si/density.h>
#include <units/isq/si/speed.h>

namespace ra {

using namespace units::isq;

[[nodiscard]] auto
densityOfAir(si::thermodynamic_temperature<si::kelvin> temperature, si::pressure<si::pascal> pressure) noexcept
    -> si::density<si::kilogram_per_metre_cub>;
[[nodiscard]] auto soundVelocity(si::thermodynamic_temperature<si::kelvin> temperature
) noexcept -> si::speed<si::metre_per_second>;
[[nodiscard]] auto
impedanceOfAir(si::thermodynamic_temperature<si::kelvin> temperature, si::pressure<si::pascal> pressure) noexcept
    -> double;

}  // namespace ra
