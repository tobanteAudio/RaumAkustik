#include "air.hpp"

#include <ra/unit/heat_capacity.hpp>

#include <units/math.h>

namespace ra {
auto densityOfAir(si::thermodynamic_temperature<si::kelvin> temp, si::pressure<si::pascal> pressure) noexcept
    -> si::density<si::kilogram_per_metre_cub>
{
    return pressure / (GasConstant<double> * temp);
}

auto soundVelocity(si::thermodynamic_temperature<si::kelvin> temp) noexcept -> si::speed<si::metre_per_second>
{
    static constexpr auto DensityAtZeroC = si::density<si::kilogram_per_metre_cub>{1.293};
    return sqrt((SpecificHeatRatio * OneAtmosphere<double>) / DensityAtZeroC)
         * sqrt(temp / si::thermodynamic_temperature<si::kelvin>{273.15});
}

auto impedanceOfAir(si::thermodynamic_temperature<si::kelvin> temp, si::pressure<si::pascal> pressure) noexcept
    -> double
{
    auto const i = soundVelocity(temp) * densityOfAir(temp, pressure);
    return i.number();
}

}  // namespace ra
