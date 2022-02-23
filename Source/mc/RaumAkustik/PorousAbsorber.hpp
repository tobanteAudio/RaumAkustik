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
    std::complex<double> zca {0};

    /// Complex wavenumber (k)
    /// Eq 5.10
    std::complex<double> k {0};

    /// Y component of wavenumber (ky)
    /// Eq 5.28
    double ky {0};

    /// X component of wavenumber (kx)
    /// Eq 5.29
    std::complex<double> kx {0};

    /// Angle of propagation in porous layer (bporous)
    /// Eq 5.28
    double betaPorous {0};

    /// Ratio of wavenumbers (k:kx)
    double ratiooOfWaveNumbers {0};
};

[[nodiscard]] auto propertiesOfAbsorber(PorousAbsorberSpecs specs, AtmosphericEnvironment env, double frequency,
                                        double angle) -> PorousAbsorberProperties;

namespace detail
{

[[nodiscard]] auto hertzToAngular(double hertz) -> double;
[[nodiscard]] auto waveNumber(double temperature, double frequency) -> double;
[[nodiscard]] auto delanyBazleyTerm(double airDensity, double frequency, double flowResistivity) -> double;
[[nodiscard]] auto yComponentOfWaveNumber(double waveNumber, double angle) -> double;
[[nodiscard]] auto angleOfPropagation(std::complex<double> k, double ky) -> double;

}  // namespace detail

inline auto oactaveSubdivision(double startFrequencyHertz, std::size_t octaveSubdivisions, std::size_t idx) -> double
{
    return std::pow(2.0, std::log2(startFrequencyHertz)
                             + static_cast<double>(idx) / static_cast<double>(octaveSubdivisions));
}

inline auto complexWaveNumber(double temperature, double pressure, double frequency, double flowResistivity)
    -> std::complex<double>
{
    // Eq 5.10
    // =IMPRODUCT(TwoPiByC,frequency,COMPLEX(1+0.0978*(X^-0.7),-0.189*(X^-0.595),"j"))
    auto const X      = detail::delanyBazleyTerm(densityOfAir(temperature, pressure), frequency, flowResistivity);
    auto const twoPiC = (2.0 * M_PI) / soundVelocity(temperature);
    return twoPiC * frequency * std::complex {1 + 0.0978 * std::pow(X, -0.7), -0.189 * std::pow(X, -0.595)};
}

inline auto xComponentOfWaveNumber(double temperature, double pressure, double frequency, double flowResistivity,
                                   double angle)
{
    auto const zca = complexWaveNumber(temperature, pressure, frequency, flowResistivity);
    auto const y   = detail::yComponentOfWaveNumber(detail::waveNumber(temperature, frequency), angle);
    return std::sqrt((zca * zca) - std::pow(y, 2.0));
};

inline auto ratioOfWaveNumbers(double temperature, double pressure, double frequency, double flowResistivity,
                               double angle)
{
    auto const zca = complexWaveNumber(temperature, pressure, frequency, flowResistivity);
    auto const y   = xComponentOfWaveNumber(temperature, pressure, frequency, flowResistivity, angle);
    return zca / y;
};

}  // namespace mc
