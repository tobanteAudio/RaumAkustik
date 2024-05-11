#pragma once

#include <units/isq/si/density.h>

namespace ra
{
using namespace units::isq;

template<typename T>
using KilogramPerCubicMetre = si::density<si::kilogram_per_metre_cub, T>;

template<typename T>
inline constexpr auto DensityAtZeroC = KilogramPerCubicMetre<T>{T(1.293)};

}  // namespace ra
