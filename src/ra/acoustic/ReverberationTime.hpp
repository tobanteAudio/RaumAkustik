#pragma once

#include <ra/unit/unit.hpp>

#include <algorithm>
#include <cmath>

namespace ra {

/// Recommended reverberation time. See EBU tech. 3276
///
/// T_m = 0.25 * (V/V0)^(1/3)
/// 0.2 < T_m < 0.4
///
/// Audio Metering: Measurements, Standards, and Practice: Chapter 32 (978-1-315-69415-3)
[[nodiscard]] auto nominalReverberationTime(QuantityOf<isq::volume> auto V) noexcept -> QuantityOf<isq::duration> auto
{
    auto const V0  = 100.0;  // m3
    auto const T_m = 0.25 * std::pow(V.numerical_value_in(cubic(si::metre)) / V0, 1.0 / 3.0);
    return std::clamp(T_m, 0.2, 0.4) * si::second;
}

}  // namespace ra
