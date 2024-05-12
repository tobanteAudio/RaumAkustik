
#include "air.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: densityOfAir", "")
{
    auto c20 = ra::Kelvin<double>{273.15 + 20.0};
    auto c22 = ra::Kelvin<double>{273.15 + 22.0};
    auto p   = ra::OneAtmosphere<double>;

    REQUIRE(
        ra::densityOfAir(c20, p).number()
        == Catch::Approx(ra::si::density<ra::si::kilogram_per_metre_cub>{1.20412}.number())
    );
    REQUIRE(
        ra::densityOfAir(c22, p).number()
        == Catch::Approx(ra::si::density<ra::si::kilogram_per_metre_cub>{1.19596}.number())
    );
}

TEST_CASE("RaumAkustik: soundVelocity", "")
{
    REQUIRE(ra::soundVelocity(ra::celciusToKelvin(20.0)).number() == Catch::Approx(343.38));
    REQUIRE(ra::soundVelocity(ra::celciusToKelvin(22.0)).number() == Catch::Approx(344.55));
}

TEST_CASE("RaumAkustik: impedanceOfAir", "")
{
    auto c20 = ra::Kelvin<double>{273.15 + 20.0};
    auto c22 = ra::Kelvin<double>{273.15 + 22.0};
    auto p   = ra::OneAtmosphere<double>;
    REQUIRE(ra::impedanceOfAir(c20, p) == Catch::Approx(413.47));
    REQUIRE(ra::impedanceOfAir(c22, p) == Catch::Approx(412.07));
}
