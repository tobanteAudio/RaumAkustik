#pragma once

#include <units/isq/si/density.h>

namespace mc
{
using namespace units::isq;
using KilogramPerMetreCub            = si::density<si::kilogram_per_metre_cub, double>;
inline constexpr auto DensityAtZeroC = KilogramPerMetreCub {1.293};
}  // namespace mc
