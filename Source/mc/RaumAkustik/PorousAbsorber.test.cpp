#include "PorousAbsorber.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: propertiesOfAbsorber", "[RaumAkustik]")
{

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0}, {20.0, 1.0}, 50.0, 0.0);
        REQUIRE(props.X == Catch::Approx(0.0075257395));
        REQUIRE(props.zca.real() == Catch::Approx(1355.71904994104));
        REQUIRE(props.zca.imag() == Catch::Approx(-1289.23418591288));
        REQUIRE(props.k.real() == Catch::Approx(3.65727296960812));
        REQUIRE(props.k.imag() == Catch::Approx(-3.17166651642184));
        REQUIRE(props.ky == Catch::Approx(0));
        REQUIRE(props.kx.real() == Catch::Approx(3.65727296960812));
        REQUIRE(props.kx.imag() == Catch::Approx(-3.17166651642184));
    }

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0}, {22.0, 1.0}, 50.0, 0.0);
        REQUIRE(props.X == Catch::Approx(0.0074747434));
        REQUIRE(props.zca.real() == Catch::Approx(1355.9444611324));
        REQUIRE(props.zca.imag() == Catch::Approx(-1291.26947073563));
        REQUIRE(props.k.real() == Catch::Approx(3.65789970046543));
        REQUIRE(props.k.imag() == Catch::Approx(-3.17371585732575));
    }
}

TEST_CASE("RaumAkustik: detail::hertzToAngular", "[RaumAkustik]")
{
    REQUIRE(mc::detail::hertzToAngular(0.0) == Catch::Approx(0.0));
    REQUIRE(mc::detail::hertzToAngular(50.0) == Catch::Approx(314.15927));
    REQUIRE(mc::detail::hertzToAngular(53.0) == Catch::Approx(333.00882));
    REQUIRE(mc::detail::hertzToAngular(56.0) == Catch::Approx(351.85838));
    REQUIRE(mc::detail::hertzToAngular(59.0) == Catch::Approx(370.70793));
}

TEST_CASE("RaumAkustik: detail::waveNumber", "[RaumAkustik]")
{
    REQUIRE(mc::detail::waveNumber(20.0, 50.0) == Catch::Approx(0.9149));
    REQUIRE(mc::detail::waveNumber(22.0, 50.0) == Catch::Approx(0.9118));
}

TEST_CASE("RaumAkustik: detail::delanyBazleyTerm", "[RaumAkustik]")
{
    REQUIRE(mc::detail::delanyBazleyTerm(mc::densityOfAir(20.0, 1.0), 50.0, 8'000.0) == Catch::Approx(0.0075257395));
    REQUIRE(mc::detail::delanyBazleyTerm(mc::densityOfAir(22.0, 1.0), 50.0, 8'000.0) == Catch::Approx(0.0074747434));
}

TEST_CASE("RaumAkustik: detail::yComponentOfWaveNumber", "[RaumAkustik]")
{
    using namespace mc::detail;
    REQUIRE(yComponentOfWaveNumber(waveNumber(20.0, 50.0), 0.0) == Catch::Approx(0.0));
    REQUIRE(yComponentOfWaveNumber(waveNumber(22.0, 50.0), 0.0) == Catch::Approx(0.0));

    REQUIRE(yComponentOfWaveNumber(waveNumber(20.0, 50.0), 15.0) == Catch::Approx(0.2367929161));
    REQUIRE(yComponentOfWaveNumber(waveNumber(22.0, 50.0), 15.0) == Catch::Approx(0.2359892724));
}

TEST_CASE("RaumAkustik: detail::angleOfPropagation", "[RaumAkustik]")
{
    auto const specs = mc::PorousAbsorberSpecs {100.0, 8'000.0};
    REQUIRE(mc::propertiesOfAbsorber(specs, {20.0, 1.0}, 50.0, 0.0).betaPorous == Catch::Approx(0.0));
    REQUIRE(mc::propertiesOfAbsorber(specs, {22.0, 1.0}, 50.0, 0.0).betaPorous == Catch::Approx(0.0));
    REQUIRE(mc::propertiesOfAbsorber(specs, {20.0, 1.0}, 50.0, 15.0).betaPorous == Catch::Approx(2.8036973865));
    REQUIRE(mc::propertiesOfAbsorber(specs, {22.0, 1.0}, 50.0, 15.0).betaPorous == Catch::Approx(2.7931256656));
}
