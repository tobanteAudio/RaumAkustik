#include "Air.hpp"

#include <mc/unit/heat_capacity.hpp>

#include <units/math.h>

namespace mc
{
auto densityOfAir(Kelvin temperature, Pascal pressure) noexcept -> KilogramPerCubicMetre
{
    return pressure / (GasConstant * temperature);
}

auto soundVelocity(Kelvin temperature) noexcept -> MetrePerSecond
{
    return sqrt((SpecificHeatRatio * OneAtmosphere) / DensityAtZeroC) * sqrt(temperature / Kelvin{273.15});
}

auto impedanceOfAir(Kelvin temperature, Pascal pressure) noexcept -> double
{
    auto const i = soundVelocity(temperature) * densityOfAir(temperature, pressure);
    return i.number();
}

}  // namespace mc
