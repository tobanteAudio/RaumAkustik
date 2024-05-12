#include "air.hpp"

#include <ra/unit/heat_capacity.hpp>

#include <units/math.h>

namespace ra {
auto densityOfAir(Kelvin<double> temp, Pascal<double> pressure) noexcept -> si::density<si::kilogram_per_metre_cub>
{
    return pressure / (GasConstant<double> * temp);
}

auto soundVelocity(Kelvin<double> temp) noexcept -> MetrePerSecond<double>
{
    static constexpr auto DensityAtZeroC = si::density<si::kilogram_per_metre_cub>{1.293};
    return sqrt((SpecificHeatRatio * OneAtmosphere<double>) / DensityAtZeroC) * sqrt(temp / Kelvin<double>{273.15});
}

auto impedanceOfAir(Kelvin<double> temp, Pascal<double> pressure) noexcept -> double
{
    auto const i = soundVelocity(temp) * densityOfAir(temp, pressure);
    return i.number();
}

}  // namespace ra
