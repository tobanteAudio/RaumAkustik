#pragma once

#include <units/isq/si/pressure.h>

namespace mc
{
using namespace units::isq;
using Pascal                        = si::pressure<si::pascal, double>;
inline constexpr auto OneAtmosphere = Pascal{101325.0};
}  // namespace mc
