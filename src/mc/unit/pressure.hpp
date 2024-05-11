#pragma once

#include <units/isq/si/pressure.h>

namespace ra {
using namespace units::isq;

template<typename T>
using Pascal = si::pressure<si::pascal, T>;

template<typename T>
inline constexpr auto OneAtmosphere = Pascal<T>{T(101325.0)};
}  // namespace ra
