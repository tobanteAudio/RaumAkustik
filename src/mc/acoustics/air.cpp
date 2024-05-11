#include "air.hpp"

#include <mc/unit/heat_capacity.hpp>

#include <units/math.h>

namespace ra
{
auto densityOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept -> KilogramPerCubicMetre<double>
{
    return pressure / (GasConstant<double> * temperature);
}

auto soundVelocity(Kelvin<double> temperature) noexcept -> MetrePerSecond<double>
{
    return sqrt((SpecificHeatRatio * OneAtmosphere<double>) / DensityAtZeroC<double>)
           * sqrt(temperature / Kelvin<double>{273.15});
}

auto impedanceOfAir(Kelvin<double> temperature, Pascal<double> pressure) noexcept -> double
{
    auto const i = soundVelocity(temperature) * densityOfAir(temperature, pressure);
    return i.number();
}

}  // namespace ra
