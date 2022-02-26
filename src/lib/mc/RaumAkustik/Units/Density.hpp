#pragma once

#include <units/isq/si/density.h>

namespace mc
{
using namespace units::isq;
using KilogramPerCubicMetre          = si::density<si::kilogram_per_metre_cub, double>;
inline constexpr auto DensityAtZeroC = KilogramPerCubicMetre {1.293};
}  // namespace mc
