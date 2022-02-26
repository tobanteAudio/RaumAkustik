#include "Air.hpp"

namespace mc
{
auto densityOfAir(Kelvin temperature, Pascal pressure) noexcept -> KilogramPerMetreCub
{
    return pressure / (GasConstant * temperature);
}

auto soundVelocity(Kelvin temperature) noexcept -> MetrePerSecond
{
    auto x = (SpecificHeatRatio * OneAtmosphere) / DensityAtZeroC;
    auto t = temperature / 273.15;
    return MetrePerSecond {std::sqrt(x.number()) * std::sqrt(t.number())};
}

auto impedanceOfAir(Kelvin temperature, Pascal pressure) noexcept -> double
{
    auto const i = soundVelocity(temperature) * densityOfAir(temperature, pressure);
    return i.number();
}

}  // namespace mc
