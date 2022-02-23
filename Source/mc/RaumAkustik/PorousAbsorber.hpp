#pragma once

#include "mc/RaumAkustik/Air.hpp"

#include <complex>

namespace mc
{
struct PorousAbsorberSpecs
{
    /// Absorber thickness (ta)
    double thickness {0};

    /// Absorber flow resisitivity
    double flowResisitivity {0};
};

struct PorousAbsorberProperties
{
    /// Delany & Bazley's term (X)
    /// Eq 5.11
    double X {0};

    /// Characteristic absorber impedance (zca)
    /// Eq 5.9
    double zca {0};

    /// Complex wavenumber (k)
    /// Eq 5.10
    double k {0};

    /// Y component of wavenumber (ky)
    /// Eq 5.28
    double ky {0};

    /// X component of wavenumber (kx)
    /// Eq 5.29
    double kx {0};

    /// Angle of propagation in porous layer (bporous)
    /// Eq 5.28
    double betaPorous {0};

    /// Ratio of wavenumbers (k:kx)
    double ratiooOfWaveNumbers {0};
};

template<typename T>
constexpr auto hertzToAngular(T hertz) -> T
{
    // 2pf
    return (T(2) * static_cast<T>(M_PI)) * hertz;
}
template<typename T>
constexpr auto oactaveSubdivision(T startFrequencyHertz, std::size_t octaveSubdivisions, std::size_t idx) -> T
{
    return std::pow(T(2), std::log2(startFrequencyHertz) + static_cast<T>(idx) / static_cast<T>(octaveSubdivisions));
}

template<typename T>
constexpr auto waveNumber(T temperature, T frequency) -> T
{
    // 2p/l
    return ((T(2) * static_cast<T>(M_PI)) / soundVelocity(temperature)) * frequency;
}

template<typename T>
constexpr auto delanyBazleyTerm(T temperature, T pressure, T frequency, T flowResistivity) -> T
{
    // Eq 5.11
    return (densityOfAir(temperature, pressure) * frequency) / flowResistivity;
}

template<typename T>
constexpr auto characteristicAbsorberImpedance(T temperature, T pressure, T frequency, T flowResistivity)
    -> std::complex<T>
{
    // Eq 5.9
    // =IMPRODUCT(airImpedance,COMPLEX(1+0.0571*(Q13^-0.754),-0.087*(Q13^-0.732),"j"))
    auto const X = delanyBazleyTerm(temperature, pressure, frequency, flowResistivity);
    return impedanceOfAir(temperature, pressure)
           * std::complex {
               1 + 0.0571 * (std::pow(X, -0.754)),
               -0.087 * (std::pow(X, -0.732)),
           };
}

template<typename T>
constexpr auto complexWaveNumber(T temperature, T pressure, T frequency, T flowResistivity) -> std::complex<T>
{
    // Eq 5.10
    // =IMPRODUCT(TwoPiByC,frequency,COMPLEX(1+0.0978*(X^-0.7),-0.189*(X^-0.595),"j"))
    auto const X      = delanyBazleyTerm(temperature, pressure, frequency, flowResistivity);
    auto const twoPiC = (T(2) * static_cast<T>(M_PI)) / soundVelocity(temperature);
    return twoPiC * frequency * std::complex {1 + 0.0978 * std::pow(X, -0.7), -0.189 * std::pow(X, -0.595)};
}

template<typename T>
inline auto yComponentOfWaveNumber(T waveNumber, T angle) -> T
{
    // =waveNumber*SIN(angle*PI()/180)
    return waveNumber * std::sin(angle * static_cast<T>(M_PI) / T(180));
};

template<typename T>
inline auto xComponentOfWaveNumber(T temperature, T pressure, T frequency, T flowResistivity, T angle)
{
    auto const zca = complexWaveNumber(temperature, pressure, frequency, flowResistivity);
    auto const y   = yComponentOfWaveNumber(waveNumber(temperature, frequency), angle);
    return std::sqrt((zca * zca) - std::pow(y, T(2)));
};

template<typename T>
inline auto angleOfPropagation(T temperature, T pressure, T frequency, T flowResistivity, T angle) -> T
{
    auto const zca = complexWaveNumber(temperature, pressure, frequency, flowResistivity);
    auto const y   = yComponentOfWaveNumber(waveNumber(temperature, frequency), angle);
    return std::asin(std::abs(y / zca)) * T(180) / static_cast<T>(M_PI);
};

template<typename T>
inline auto ratioOfWaveNumbers(T temperature, T pressure, T frequency, T flowResistivity, T angle)
{
    auto const zca = complexWaveNumber(temperature, pressure, frequency, flowResistivity);
    auto const y   = xComponentOfWaveNumber(temperature, pressure, frequency, flowResistivity, angle);
    return zca / y;
};

}  // namespace mc
