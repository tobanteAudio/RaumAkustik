#include "porous_absorber.hpp"

#include <mc/acoustics/Air.hpp>

#include <numbers>

namespace mc
{
auto propertiesOfAbsorber(PorousAbsorberSpecs specs, AtmosphericEnvironment env, Hertz<double> frequency, double angle)
    -> PorousAbsorberProperties
{
    auto const airDensity = densityOfAir(env.temperature, env.pressure);
    auto const airIm      = impedanceOfAir(env.temperature, env.pressure);
    auto const twoPiC     = (2.0 * std::numbers::pi) / soundVelocity(env.temperature).number();
    auto const wn         = detail::waveNumber(env.temperature, frequency);
    auto const nj         = std::complex{0.0, -1.0};

    auto p = PorousAbsorberProperties{};
    {
        p.X   = detail::delanyBazleyTerm(airDensity, frequency, specs.flowResisitivity);
        p.zca = airIm * std::complex{1 + 0.0571 * (std::pow(p.X, -0.754)), -0.087 * (std::pow(p.X, -0.732))};
        p.k   = twoPiC * frequency.number()
              * std::complex{1 + 0.0978 * std::pow(p.X, -0.7), -0.189 * std::pow(p.X, -0.595)};
        p.ky                  = detail::yComponentOfWaveNumber(wn, angle);
        p.kx                  = std::sqrt((p.k * p.k) - std::pow(p.ky, 2));
        p.betaPorous          = detail::angleOfPropagation(p.k, p.ky);
        p.ratiooOfWaveNumbers = p.k / p.kx;

        // Impedance
        auto const tmp               = p.k * specs.thickness / 1'000.0;
        p.impedance.intermediateTerm = std::cos(tmp) / std::sin(tmp);
        p.impedance.atSurface        = nj * p.zca * p.ratiooOfWaveNumbers * p.impedance.intermediateTerm;

        // No air gap
        auto const tap             = (p.impedance.atSurface / airIm) * std::cos(angle * std::numbers::pi / 180.0);
        p.reflectionFactorNoAirGap = (tap - 1.0) / (tap + 1.0);
        p.absorptionFactorNoAirGap = 1.0 - std::pow(std::abs(p.reflectionFactorNoAirGap), 2.0);
    }

    {
        // with air gap
        p.kAirY     = p.k * std::sin(p.betaPorous * std::numbers::pi / 180.0);
        p.kAirX     = std::sqrt(std::pow(wn, 2.0) - (p.kAirY * p.kAirY));
        p.betaAir   = std::asin(std::abs(p.kAirY / airIm)) * 180.0 / std::numbers::pi;
        p.kAirRatio = wn / p.kAirX;
        p.zAir      = nj * airIm * p.kAirRatio
                 * (std::cos(wn * (specs.airGap / 1'000.0)) / std::sin(wn * (specs.airGap / 1'000.0)));
        p.ki = nj * p.zca * p.impedance.intermediateTerm;

        p.zaAir = ((p.ki * p.zAir) + (p.zca * p.zca)) / (p.zAir + p.ki);

        auto const tmp               = (p.zaAir / airIm) * std::cos(angle * std::numbers::pi / 180.0);
        p.reflectionFactorWithAirGap = (tmp - 1.0) / (tmp + 1.0);
        p.absorptionFactorWithAirGap = 1.0 - std::pow(std::abs(p.reflectionFactorWithAirGap), 2.0);
    }

    return p;
}

namespace detail
{

auto waveNumber(Kelvin<double> temperature, Hertz<double> frequency) -> double
{
    // 2p/l
    return ((2.0 * std::numbers::pi) / soundVelocity(temperature).number()) * frequency.number();
}
auto delanyBazleyTerm(KilogramPerCubicMetre<double> airDensity, Hertz<double> frequency, double flowResistivity)
    -> double
{
    // Eq 5.11
    auto const tmp = (airDensity * frequency) / flowResistivity;
    return tmp.number();
}

auto yComponentOfWaveNumber(double waveNumber, double angle) -> double
{
    // =waveNumber*SIN(angle*PI()/180)
    return waveNumber * std::sin(angle * std::numbers::pi / 180.0);
}

auto angleOfPropagation(std::complex<double> k, double ky) -> double
{
    return std::asin(std::abs(ky / k)) * 180.0 / std::numbers::pi;
}

}  // namespace detail
}  // namespace mc
