#pragma once

#include <ra/unit/density.hpp>
#include <ra/unit/frequency.hpp>
#include <ra/unit/pressure.hpp>
#include <ra/unit/temperature.hpp>

#include <complex>

namespace ra {
struct AtmosphericEnvironment
{
    Kelvin<double> temperature{0};
    Pascal<double> pressure{0};
};

struct PorousAbsorberSpecs
{
    /// Absorber thickness (ta)
    double thickness{0};

    /// Absorber flow resisitivity
    double flowResisitivity{0};

    /// Gap between absorber and rigid surface (mm)
    double airGap{0};
};

struct PorousAbsorberImpedance
{
    // Intermediate term cot(k*ta)
    std::complex<double> intermediateTerm;

    // Impedance at absorber surface (zsa)
    std::complex<double> atSurface;
};

struct PorousAbsorberProperties
{
    /// Delany & Bazley's term (X)
    /// Eq 5.11
    double X{0};

    /// Characteristic absorber impedance (zca)
    /// Eq 5.9
    std::complex<double> zca{0};

    /// Complex wavenumber (k)
    /// Eq 5.10
    std::complex<double> k{0};

    /// Y component of wavenumber (ky)
    /// Eq 5.28
    double ky{0};

    /// X component of wavenumber (kx)
    /// Eq 5.29
    std::complex<double> kx{0};

    /// Angle of propagation in porous layer (bporous)
    /// Eq 5.28
    double betaPorous{0};

    /// Ratio of wavenumbers (k:kx)
    std::complex<double> ratiooOfWaveNumbers{0};

    PorousAbsorberImpedance impedance{};

    std::complex<double> reflectionFactorNoAirGap;
    double absorptionFactorNoAirGap{};

    std::complex<double> kAirY{0.0};
    std::complex<double> kAirX{0.0};
    double betaAir{0};
    std::complex<double> kAirRatio{0};
    std::complex<double> zAir{0};
    std::complex<double> ki{0};
    std::complex<double> zaAir{0};

    std::complex<double> reflectionFactorWithAirGap;
    double absorptionFactorWithAirGap{};
};

[[nodiscard]] auto
propertiesOfAbsorber(PorousAbsorberSpecs specs, AtmosphericEnvironment env, Hertz<double> frequency, double angle)
    -> PorousAbsorberProperties;

namespace detail {

[[nodiscard]] auto waveNumber(Kelvin<double> temperature, Hertz<double> frequency) -> double;
[[nodiscard]] auto
delanyBazleyTerm(KilogramPerCubicMetre<double> airDensity, Hertz<double> frequency, double flowResistivity) -> double;
[[nodiscard]] auto yComponentOfWaveNumber(double waveNumber, double angle) -> double;
[[nodiscard]] auto angleOfPropagation(std::complex<double> k, double ky) -> double;

}  // namespace detail

}  // namespace ra
