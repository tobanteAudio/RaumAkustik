#include "PorousAbsorber.hpp"

#include <ra/acoustic/Air.hpp>
#include <ra/unit/pressure.hpp>
#include <ra/unit/temperature.hpp>

#include <numbers>

namespace ra {
auto propertiesOfAbsorber(
    PorousAbsorberSpecs specs,
    AtmosphericEnvironment env,
    quantity<isq::frequency[si::hertz]> frequency,
    double angle
) -> PorousAbsorberProperties
{
    using namespace mp_units::si::unit_symbols;

    auto const airDensity = densityOfAir(env.temperature, env.pressure);
    auto const airIm      = impedanceOfAir(env.temperature, env.pressure);
    auto const twoPiC     = (2.0 * std::numbers::pi) / soundVelocity(env.temperature).numerical_value_in(m / s);
    auto const wn         = detail::waveNumber(env.temperature, frequency);
    auto const nj         = std::complex{0.0, -1.0};

    auto p = PorousAbsorberProperties{};
    {
        p.X   = detail::delanyBazleyTerm(airDensity, frequency, specs.flowResisitivity);
        p.zca = airIm * std::complex{1 + 0.0571 * (std::pow(p.X, -0.754)), -0.087 * (std::pow(p.X, -0.732))};
        p.k   = twoPiC * frequency.numerical_value_in(Hz)
            * std::complex{1 + 0.0978 * std::pow(p.X, -0.7), -0.189 * std::pow(p.X, -0.595)};
        p.ky                  = detail::yComponentOfWaveNumber(wn, angle);
        p.kx                  = std::sqrt((p.k * p.k) - std::pow(p.ky, 2));
        p.betaPorous          = detail::angleOfPropagation(p.k, p.ky);
        p.ratiooOfWaveNumbers = p.k / p.kx;

        // Impedance
        auto const tmp               = p.k * specs.thickness.numerical_value_in(si::metre);
        p.impedance.intermediateTerm = std::cos(tmp) / std::sin(tmp);
        p.impedance.atSurface        = nj * p.zca * p.ratiooOfWaveNumbers * p.impedance.intermediateTerm;

        // No air gap
        auto const tap             = (p.impedance.atSurface / airIm) * std::cos(angle * std::numbers::pi / 180.0);
        p.reflectionFactorNoAirGap = (tap - 1.0) / (tap + 1.0);
        p.absorptionFactorNoAirGap = 1.0 - std::pow(std::abs(p.reflectionFactorNoAirGap), 2.0);
    }

    {
        // with air gap
        auto const airGap = specs.airGap.numerical_value_in(si::metre);

        p.kAirY     = p.k * std::sin(p.betaPorous * std::numbers::pi / 180.0);
        p.kAirX     = std::sqrt(std::pow(wn, 2.0) - (p.kAirY * p.kAirY));
        p.betaAir   = std::asin(std::abs(p.kAirY / airIm)) * 180.0 / std::numbers::pi;
        p.kAirRatio = wn / p.kAirX;
        p.zAir      = nj * airIm * p.kAirRatio * (std::cos(wn * (airGap)) / std::sin(wn * (airGap)));
        p.ki        = nj * p.zca * p.impedance.intermediateTerm;

        p.zaAir = ((p.ki * p.zAir) + (p.zca * p.zca)) / (p.zAir + p.ki);

        auto const tmp               = (p.zaAir / airIm) * std::cos(angle * std::numbers::pi / 180.0);
        p.reflectionFactorWithAirGap = (tmp - 1.0) / (tmp + 1.0);
        p.absorptionFactorWithAirGap = 1.0 - std::pow(std::abs(p.reflectionFactorWithAirGap), 2.0);
    }

    return p;
}

namespace detail {

auto waveNumber(
    quantity<isq::thermodynamic_temperature[si::kelvin]> temperature,
    quantity<isq::frequency[si::hertz]> frequency
) -> double
{
    // 2p/l
    using namespace mp_units::si::unit_symbols;
    return ((2.0 * std::numbers::pi) / soundVelocity(temperature).numerical_value_in(m / s))
         * frequency.numerical_value_in(Hz);
}

auto delanyBazleyTerm(
    quantity<isq::density[si::kilogram / cubic(si::metre)]> airDensity,
    quantity<isq::frequency[si::hertz]> frequency,
    double flowResistivity
) -> double
{
    // Eq 5.11
    using namespace mp_units::si::unit_symbols;
    auto const tmp = (airDensity * frequency) / flowResistivity;
    return tmp.numerical_value_in((kg / m3) * Hz / one);
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
}  // namespace ra
