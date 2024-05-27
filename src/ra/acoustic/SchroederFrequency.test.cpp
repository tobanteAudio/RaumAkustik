
#include "SchroederFrequency.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: schroederFrequency", "")
{
    using namespace mp_units::si::unit_symbols;

    REQUIRE(ra::schroederFrequency(1.0 * s, 100 * m3).numerical_value_in(Hz) == Catch::Approx(200.0));
    REQUIRE(ra::schroederFrequency(4.0 * s, 100 * m3).numerical_value_in(Hz) == Catch::Approx(400.0));
    REQUIRE(ra::schroederFrequency(2.5 * s, 100 * m3).numerical_value_in(Hz) == Catch::Approx(316.227766));
}
