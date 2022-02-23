#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

namespace mc
{
inline constexpr auto gasConstant       = 287.05;    // J Kg-1 °K-1
inline constexpr auto densityAtZeroC    = 1.293;     // Kinsler & Frey, Appendix A10, Table (c)
inline constexpr auto specificHeatRatio = 1.402;     // Kinsler & Frey, Appendix A10, Table (c)
inline constexpr auto oneAtmosphere     = 101325.0;  // Pa

template<typename T>
inline auto densityOfAir(T temperature, T pressure) -> T
{
    // kg m-3
    // Effects of humidity are neglected
    return (pressure * oneAtmosphere) / (gasConstant * (T(273.15) + temperature));
}

template<typename T>
inline auto soundVelocity(T temperature) -> T
{
    return std::sqrt((specificHeatRatio * oneAtmosphere) / densityAtZeroC)
           * std::sqrt(T(1) + (temperature / T(273.15)));
}

template<typename T>
inline auto impedanceOfAir(T temperature, T pressure) -> T
{
    return soundVelocity(temperature) * densityOfAir(temperature, pressure);
}

}  // namespace mc
