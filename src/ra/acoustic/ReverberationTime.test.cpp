
#include "ReverberationTime.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: nominalReverberationTime", "")
{
    using namespace mp_units::si::unit_symbols;

    REQUIRE(ra::nominalReverberationTime(6.0 * 2.0 * 1.0 * m3).numerical_value_in(s) == Catch::Approx(0.20));
    REQUIRE(ra::nominalReverberationTime(6.0 * 3.65 * 3.12 * m3).numerical_value_in(s) == Catch::Approx(0.2201943874));
    REQUIRE(ra::nominalReverberationTime(10.0 * 20.0 * 30.0 * m3).numerical_value_in(s) == Catch::Approx(0.40));
}
