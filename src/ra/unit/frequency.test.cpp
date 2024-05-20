#include "frequency.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: oactaveSubdivision", "")
{
    using namespace mp_units::si::unit_symbols;

    REQUIRE(ra::oactaveSubdivision(50.0 * Hz, 6, 1).numerical_value_in(Hz) == Catch::Approx(56.1231024));
    REQUIRE(ra::oactaveSubdivision(50.0 * Hz, 6, 2).numerical_value_in(Hz) == Catch::Approx(62.9960524));
}
