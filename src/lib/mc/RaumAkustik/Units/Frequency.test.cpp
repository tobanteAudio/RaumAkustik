#include "Frequency.hpp"

#include <catch2/catch.hpp>

TEST_CASE("RaumAkustik: oactaveSubdivision", "[RaumAkustik]")
{
    REQUIRE(mc::oactaveSubdivision(mc::Hertz {50.0}, 6, 1).number() == Approx(56.1231024));
    REQUIRE(mc::oactaveSubdivision(mc::Hertz {50.0}, 6, 2).number() == Approx(62.9960524));
}