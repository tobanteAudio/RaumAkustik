#include "Frequency.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: oactaveSubdivision", "[RaumAkustik]")
{
    REQUIRE(mc::oactaveSubdivision(mc::Hertz{50.0}, 6, 1).number() == Catch::Approx(56.1231024));
    REQUIRE(mc::oactaveSubdivision(mc::Hertz{50.0}, 6, 2).number() == Catch::Approx(62.9960524));
}

TEST_CASE("RaumAkustik: frequencyToAngularVelocity", "[RaumAkustik]")
{
    REQUIRE(mc::frequencyToAngularVelocity(mc::Hertz{0.0}).number() == Catch::Approx(0.0));
    REQUIRE(mc::frequencyToAngularVelocity(mc::Hertz{50.0}).number() == Catch::Approx(314.15927));
    REQUIRE(mc::frequencyToAngularVelocity(mc::Hertz{53.0}).number() == Catch::Approx(333.00882));
    REQUIRE(mc::frequencyToAngularVelocity(mc::Hertz{56.0}).number() == Catch::Approx(351.85838));
    REQUIRE(mc::frequencyToAngularVelocity(mc::Hertz{59.0}).number() == Catch::Approx(370.70793));
}
