#pragma once

#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

namespace ra {
using namespace mp_units;

template<typename T>
inline constexpr QuantityOf<isq::pressure> auto OneAtmosphere = static_cast<T>(101325.0) * si::pascal;

}  // namespace ra
