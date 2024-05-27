#pragma once

#include <ra/unit/unit.hpp>

namespace ra {

// T is the reverberation time
// V is the volume
[[nodiscard]] auto schroederFrequency(QuantityOf<isq::duration> auto T, QuantityOf<isq::volume> auto V) noexcept
    -> QuantityOf<isq::frequency> auto
{
    auto tmp = 2000.0 * std::sqrt(T.numerical_value_in(si::second) / V.numerical_value_in(cubic(si::metre)));
    return tmp * si::hertz;
}

}  // namespace ra
