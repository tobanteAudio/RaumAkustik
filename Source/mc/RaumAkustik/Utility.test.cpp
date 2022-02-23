#include "Utility.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: oactaveSubdivision", "[RaumAkustik]")
{
    REQUIRE(mc::oactaveSubdivision(50.0, 6, 1) == Catch::Approx(56.1231024));
    REQUIRE(mc::oactaveSubdivision(50.0, 6, 2) == Catch::Approx(62.9960524));
}