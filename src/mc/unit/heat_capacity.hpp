#pragma once

#include <units/isq/si/heat_capacity.h>

namespace ra
{

using namespace units::isq;

template<typename T>
using JoulePerKilogramKelvin = si::specific_heat_capacity<si::joule_per_kilogram_kelvin, T>;

inline constexpr auto SpecificHeatRatio = 1.402;

template<typename T>
inline constexpr auto GasConstant = JoulePerKilogramKelvin<T>{287.05};

}  // namespace ra
