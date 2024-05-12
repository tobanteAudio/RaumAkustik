#pragma once

#include <units/isq/si/pressure.h>

namespace ra {
using namespace units::isq;

template<typename T>
inline constexpr auto OneAtmosphere = si::pressure<si::pascal, T>{T(101325.0)};

}  // namespace ra
