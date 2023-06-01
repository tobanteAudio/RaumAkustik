
#include "air.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: densityOfAir", "[RaumAkustik]")
{
    auto c20 = mc::Kelvin<double>{273.15 + 20.0};
    auto c22 = mc::Kelvin<double>{273.15 + 22.0};
    auto p   = mc::OneAtmosphere<double>;

    REQUIRE(mc::densityOfAir(c20, p).number() == Catch::Approx(mc::KilogramPerCubicMetre{1.20412}.number()));
    REQUIRE(mc::densityOfAir(c22, p).number() == Catch::Approx(mc::KilogramPerCubicMetre{1.19596}.number()));
}

TEST_CASE("RaumAkustik: soundVelocity", "[RaumAkustik]")
{
    REQUIRE(mc::soundVelocity(mc::celciusToKelvin(20.0)).number() == Catch::Approx(343.38));
    REQUIRE(mc::soundVelocity(mc::celciusToKelvin(22.0)).number() == Catch::Approx(344.55));
}

TEST_CASE("RaumAkustik: impedanceOfAir", "[RaumAkustik]")
{
    auto c20 = mc::Kelvin<double>{273.15 + 20.0};
    auto c22 = mc::Kelvin<double>{273.15 + 22.0};
    auto p   = mc::OneAtmosphere<double>;
    REQUIRE(mc::impedanceOfAir(c20, p) == Catch::Approx(413.47));
    REQUIRE(mc::impedanceOfAir(c22, p) == Catch::Approx(412.07));
}
