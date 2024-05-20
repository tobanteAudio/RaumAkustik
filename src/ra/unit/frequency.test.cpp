#include "frequency.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: oactaveSubdivision", "")
{
    using namespace mp_units::si::unit_symbols;

    REQUIRE(ra::oactaveSubdivision(50.0 * Hz, 6, 1).numerical_value_in(Hz) == Catch::Approx(56.1231024));
    REQUIRE(ra::oactaveSubdivision(50.0 * Hz, 6, 2).numerical_value_in(Hz) == Catch::Approx(62.9960524));
}

TEST_CASE("RaumAkustik: toAngularVelocity", "")
{
    using namespace mp_units::si::unit_symbols;

    REQUIRE(ra::toAngularVelocity(0.0 * Hz).numerical_value_in(rad / s) == Catch::Approx(0.0));
    REQUIRE(ra::toAngularVelocity(50.0 * Hz).numerical_value_in(rad / s) == Catch::Approx(314.15927));
    REQUIRE(ra::toAngularVelocity(53.0 * Hz).numerical_value_in(rad / s) == Catch::Approx(333.00882));
    REQUIRE(ra::toAngularVelocity(56.0 * Hz).numerical_value_in(rad / s) == Catch::Approx(351.85838));
    REQUIRE(ra::toAngularVelocity(59.0 * Hz).numerical_value_in(rad / s) == Catch::Approx(370.70793));
}
