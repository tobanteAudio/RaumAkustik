#pragma once

#include <units/isq/si/heat_capacity.h>

namespace ra {

using namespace units::isq;

inline constexpr auto SpecificHeatRatio = 1.402;

template<typename T>
inline constexpr auto GasConstant = si::specific_heat_capacity<si::joule_per_kilogram_kelvin, T>{287.05};

}  // namespace ra
