#include "Frequency.hpp"

#include <catch2/catch.hpp>

TEST_CASE("RaumAkustik: oactaveSubdivision", "[RaumAkustik]")
{
    REQUIRE(mc::oactaveSubdivision(50.0, 6, 1) == Approx(56.1231024));
    REQUIRE(mc::oactaveSubdivision(50.0, 6, 2) == Approx(62.9960524));
}