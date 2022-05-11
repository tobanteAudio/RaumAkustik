
#include "Air.hpp"

#include <catch2/catch.hpp>

TEST_CASE("RaumAkustik: densityOfAir", "[RaumAkustik]")
{
    auto c20 = mc::Kelvin{273.15 + 20.0};
    auto c22 = mc::Kelvin{273.15 + 22.0};
    auto p   = mc::OneAtmosphere;

    REQUIRE(mc::densityOfAir(c20, p).number() == Approx(mc::KilogramPerCubicMetre{1.20412}.number()));
    REQUIRE(mc::densityOfAir(c22, p).number() == Approx(mc::KilogramPerCubicMetre{1.19596}.number()));
}

TEST_CASE("RaumAkustik: soundVelocity", "[RaumAkustik]")
{
    REQUIRE(mc::soundVelocity(mc::celciusToKelvin(20.0)).number() == Approx(343.38));
    REQUIRE(mc::soundVelocity(mc::celciusToKelvin(22.0)).number() == Approx(344.55));
}

TEST_CASE("RaumAkustik: impedanceOfAir", "[RaumAkustik]")
{
    auto c20 = mc::Kelvin{273.15 + 20.0};
    auto c22 = mc::Kelvin{273.15 + 22.0};
    auto p   = mc::OneAtmosphere;
    REQUIRE(mc::impedanceOfAir(c20, p) == Approx(413.47));
    REQUIRE(mc::impedanceOfAir(c22, p) == Approx(412.07));
}
