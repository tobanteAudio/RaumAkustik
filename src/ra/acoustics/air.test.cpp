
#include "air.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: densityOfAir", "")
{
    using namespace mp_units::si::unit_symbols;

    auto c20 = (273.15 + 20.0) * ra::si::kelvin;
    auto c22 = (273.15 + 22.0) * ra::si::kelvin;
    auto p   = ra::OneAtmosphere<double>;

    REQUIRE(ra::densityOfAir(c20, p).numerical_value_in(kg / m3) == Catch::Approx(1.20412));
    REQUIRE(ra::densityOfAir(c22, p).numerical_value_in(kg / m3) == Catch::Approx(1.19596));
}

TEST_CASE("RaumAkustik: soundVelocity", "")
{
    using namespace mp_units::si::unit_symbols;

    REQUIRE(ra::soundVelocity(ra::celciusToKelvin(20.0)).numerical_value_in(m / s) == Catch::Approx(343.38));
    REQUIRE(ra::soundVelocity(ra::celciusToKelvin(22.0)).numerical_value_in(m / s) == Catch::Approx(344.55));
}

TEST_CASE("RaumAkustik: impedanceOfAir", "")
{
    using namespace mp_units::si::unit_symbols;

    auto c20 = (273.15 + 20.0) * ra::si::kelvin;
    auto c22 = (273.15 + 22.0) * ra::si::kelvin;
    auto p   = ra::OneAtmosphere<double>;
    REQUIRE(ra::impedanceOfAir(c20, p) == Catch::Approx(413.47));
    REQUIRE(ra::impedanceOfAir(c22, p) == Catch::Approx(412.07));
}
