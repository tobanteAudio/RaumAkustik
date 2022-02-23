#define _USE_MATH_DEFINES
#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

namespace mc
{
struct PorousAbsorber
{
    double thickness {0};
    double flowResisitivity {0};
};
template<typename T>
constexpr auto hertzToAngular(T hertz) -> T
{
    return (T(2) * static_cast<T>(M_PI)) * hertz;
}
template<typename T>
constexpr auto oactaveSubdivision(T startFrequencyHertz, std::size_t octaveSubdivisions, std::size_t idx) -> T
{
    return std::pow(T(2), std::log2(startFrequencyHertz) + static_cast<T>(idx) / static_cast<T>(octaveSubdivisions));
}

}  // namespace mc

TEST_CASE("RaumAkustik: hertzToAngular", "[RaumAkustik]")
{
    REQUIRE(mc::hertzToAngular(0.0) == Catch::Approx(0.0));
    REQUIRE(mc::hertzToAngular(50.0) == Catch::Approx(314.15927));
    REQUIRE(mc::hertzToAngular(53.0) == Catch::Approx(333.00882));
    REQUIRE(mc::hertzToAngular(56.0) == Catch::Approx(351.85838));
    REQUIRE(mc::hertzToAngular(59.0) == Catch::Approx(370.70793));
}

TEST_CASE("RaumAkustik: oactaveSubdivision", "[RaumAkustik]")
{
    REQUIRE(mc::oactaveSubdivision(50.0, 6, 1) == Catch::Approx(56.1231024));
    REQUIRE(mc::oactaveSubdivision(50.0, 6, 2) == Catch::Approx(62.9960524));
}