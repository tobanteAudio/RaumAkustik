#pragma once

#include "mc/RaumAkustik/Units.hpp"

namespace mc
{
inline constexpr auto gasConstant       = 287.05;    // J Kg-1 °K-1
inline constexpr auto densityAtZeroC    = 1.293;     // Kinsler & Frey, Appendix A10, Table (c)
inline constexpr auto specificHeatRatio = 1.402;     // Kinsler & Frey, Appendix A10, Table (c)
inline constexpr auto oneAtmosphere     = 101325.0;  // Pa

struct AtmosphericEnvironment
{
    Kelvin temperature {0};
    Pascal pressure {0};
};

[[nodiscard]] auto densityOfAir(Kelvin temperature, Pascal pressure) noexcept -> KilogramPerMetreCub;
[[nodiscard]] auto soundVelocity(Kelvin temperature) noexcept -> MetrePerSecond;
[[nodiscard]] auto impedanceOfAir(Kelvin temperature, Pascal pressure) noexcept -> double;

}  // namespace mc
