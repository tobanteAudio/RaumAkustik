#include "air.hpp"

#include <algorithm>

#include <mp-units/math.h>

namespace ra {
auto densityOfAir(
    quantity<isq::thermodynamic_temperature[si::kelvin]> temp,
    quantity<isq::pressure[si::pascal]> pressure
) noexcept -> quantity<isq::density[si::kilogram / cubic(si::metre)]>
{
    using namespace mp_units::si::unit_symbols;

    static constexpr auto gasConstant = 287.05 * J / (kg * K);
    return pressure / (gasConstant * temp);
}

auto soundVelocity(quantity<isq::thermodynamic_temperature[si::kelvin]> temp
) noexcept -> quantity<isq::speed[si::metre / si::second]>
{
    using namespace mp_units::si::unit_symbols;

    static constexpr auto specificHeatRatio = 1.402;
    static constexpr auto densityAtZeroC    = 1.293 * si::kilogram / cubic(si::metre);
    return sqrt((specificHeatRatio * OneAtmosphere<double>) / densityAtZeroC) * sqrt(temp / (273.15 * si::kelvin));
}

auto impedanceOfAir(
    quantity<isq::thermodynamic_temperature[si::kelvin]> temp,
    quantity<isq::pressure[si::pascal]> pressure
) noexcept -> double
{
    using namespace mp_units::si::unit_symbols;
    auto const i = soundVelocity(temp) * densityOfAir(temp, pressure);
    return i.numerical_value_in((m / s) * (kg / m3));
}

}  // namespace ra
