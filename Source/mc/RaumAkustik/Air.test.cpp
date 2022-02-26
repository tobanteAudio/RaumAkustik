
#include "Air.hpp"

#include <catch2/catch.hpp>

TEST_CASE("RaumAkustik: densityOfAir", "[RaumAkustik]")
{
    REQUIRE(mc::densityOfAir(20.0, 1.0) == Approx(1.2041183));
    REQUIRE(mc::densityOfAir(22.0, 1.0) == Approx(1.1959589));
}

TEST_CASE("RaumAkustik: soundVelocity", "[RaumAkustik]")
{
    REQUIRE(mc::soundVelocity(20.0) == Approx(343.38));
    REQUIRE(mc::soundVelocity(22.0) == Approx(344.55));
}

TEST_CASE("RaumAkustik: impedanceOfAir", "[RaumAkustik]")
{
    REQUIRE(mc::impedanceOfAir(20.0, 1.0) == Approx(413.47));
    REQUIRE(mc::impedanceOfAir(22.0, 1.0) == Approx(412.07));
}
