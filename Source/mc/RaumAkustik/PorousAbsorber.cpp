#include "PorousAbsorber.hpp"

namespace mc
{
auto propertiesOfAbsorber(PorousAbsorberSpecs specs, AtmosphericEnvironment env, double frequency, double angle)
    -> PorousAbsorberProperties
{
    auto const airIm  = impedanceOfAir(env.temperature, env.pressure);
    auto const twoPiC = (2.0 * M_PI) / soundVelocity(env.temperature);

    auto p = PorousAbsorberProperties {};
    p.X    = detail::delanyBazleyTerm(densityOfAir(env.temperature, env.pressure), frequency, specs.flowResisitivity);
    p.zca  = airIm * std::complex {1 + 0.0571 * (std::pow(p.X, -0.754)), -0.087 * (std::pow(p.X, -0.732))};
    p.k    = twoPiC * frequency * std::complex {1 + 0.0978 * std::pow(p.X, -0.7), -0.189 * std::pow(p.X, -0.595)};
    p.ky   = detail::yComponentOfWaveNumber(detail::waveNumber(env.temperature, frequency), angle);
    p.kx   = std::sqrt((p.k * p.k) - std::pow(p.ky, 2));
    p.betaPorous          = detail::angleOfPropagation(p.k, p.ky);
    p.ratiooOfWaveNumbers = p.k / p.kx;

    return p;
}

namespace detail
{
auto hertzToAngular(double hertz) -> double
{
    // 2pf
    return (2.0 * M_PI) * hertz;
}

auto waveNumber(double temperature, double frequency) -> double
{
    // 2p/l
    return ((2.0 * M_PI) / soundVelocity(temperature)) * frequency;
}
auto delanyBazleyTerm(double airDensity, double frequency, double flowResistivity) -> double
{
    // Eq 5.11
    return (airDensity * frequency) / flowResistivity;
}

auto yComponentOfWaveNumber(double waveNumber, double angle) -> double
{
    // =waveNumber*SIN(angle*PI()/180)
    return waveNumber * std::sin(angle * M_PI / 180.0);
}

auto angleOfPropagation(std::complex<double> k, double ky) -> double
{
    return std::asin(std::abs(ky / k)) * 180.0 / M_PI;
}

}  // namespace detail
}  // namespace mc