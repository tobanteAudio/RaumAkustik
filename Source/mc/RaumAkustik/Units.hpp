#pragma once

#include <units/isq/si/density.h>
#include <units/isq/si/heat_capacity.h>
#include <units/isq/si/length.h>
#include <units/isq/si/pressure.h>
#include <units/isq/si/thermodynamic_temperature.h>
#include <units/isq/si/time.h>

namespace mc
{

using namespace units::isq;

using Kelvin                 = si::thermodynamic_temperature<si::kelvin, double>;
using Pascal                 = si::pressure<si::pascal, double>;
using KilogramPerMetreCub    = si::density<si::kilogram_per_metre_cub, double>;
using MetrePerSecond         = si::speed<si::metre_per_second, double>;
using JoulePerKilogramKelvin = si::specific_heat_capacity<si::joule_per_kilogram_kelvin, double>;

inline constexpr auto DensityAtZeroC    = KilogramPerMetreCub {1.293};
inline constexpr auto SpecificHeatRatio = 1.402;
inline constexpr auto OneAtmosphere     = Pascal {101325.0};
inline constexpr auto GasConstant       = JoulePerKilogramKelvin {287.05};

[[nodiscard]] constexpr auto celciusToKelvin(double celcius) noexcept -> Kelvin { return Kelvin {celcius + 273.15}; }
}  // namespace mc
