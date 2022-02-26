#pragma once

#include <units/isq/si/heat_capacity.h>

namespace mc
{

using namespace units::isq;
using JoulePerKilogramKelvin            = si::specific_heat_capacity<si::joule_per_kilogram_kelvin, double>;
inline constexpr auto SpecificHeatRatio = 1.402;
inline constexpr auto GasConstant       = JoulePerKilogramKelvin {287.05};

}  // namespace mc
