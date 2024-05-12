#include "frequency.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: oactaveSubdivision", "")
{
    REQUIRE(ra::oactaveSubdivision(ra::Hertz<double>{50.0}, 6, 1).number() == Catch::Approx(56.1231024));
    REQUIRE(ra::oactaveSubdivision(ra::Hertz<double>{50.0}, 6, 2).number() == Catch::Approx(62.9960524));
}

TEST_CASE("RaumAkustik: toAngularVelocity", "")
{
    REQUIRE(ra::toAngularVelocity(ra::Hertz<double>{0.0}).number() == Catch::Approx(0.0));
    REQUIRE(ra::toAngularVelocity(ra::Hertz<double>{50.0}).number() == Catch::Approx(314.15927));
    REQUIRE(ra::toAngularVelocity(ra::Hertz<double>{53.0}).number() == Catch::Approx(333.00882));
    REQUIRE(ra::toAngularVelocity(ra::Hertz<double>{56.0}).number() == Catch::Approx(351.85838));
    REQUIRE(ra::toAngularVelocity(ra::Hertz<double>{59.0}).number() == Catch::Approx(370.70793));
}
