#include "PorousAbsorber.hpp"

#include <catch2/catch.hpp>

TEST_CASE("RaumAkustik: propertiesOfAbsorber", "[RaumAkustik]")
{

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0, 100.0}, {20.0, 1.0}, 50.0, 0.0);
        REQUIRE(props.X == Approx(0.0075257395));
        REQUIRE(props.zca.real() == Approx(1355.71904994104));
        REQUIRE(props.zca.imag() == Approx(-1289.23418591288));
        REQUIRE(props.k.real() == Approx(3.65727296960812));
        REQUIRE(props.k.imag() == Approx(-3.17166651642184));
        REQUIRE(props.ky == Approx(0));
        REQUIRE(props.kx.real() == Approx(3.65727296960812));
        REQUIRE(props.kx.imag() == Approx(-3.17166651642184));

        REQUIRE(props.impedance.intermediateTerm.real() == Approx(1.4401025724));
        REQUIRE(props.impedance.intermediateTerm.imag() == Approx(1.4611988329));
        REQUIRE(props.impedance.atSurface.real() == Approx(124.3456259498));
        REQUIRE(props.impedance.atSurface.imag() == Approx(-3836.20));

        REQUIRE(props.reflectionFactorNoAirGap.real() == Approx(+0.970361524973876));
        REQUIRE(props.reflectionFactorNoAirGap.imag() == Approx(-0.211408253330928));
        REQUIRE(props.absorptionFactorNoAirGap == Approx(0.0137050613));

        REQUIRE(props.kAirY.real() == Approx(0.0));
        REQUIRE(props.kAirY.imag() == Approx(0.0));

        REQUIRE(props.kAirX.real() == Approx(0.914897572490173));
        REQUIRE(props.kAirX.imag() == Approx(0.0));

        REQUIRE(props.betaAir == Approx(0.0));

        REQUIRE(props.kAirRatio.real() == Approx(1.0));
        REQUIRE(props.kAirRatio.imag() == Approx(0.0));

        REQUIRE(props.zAir.real() == Approx(0.0));
        REQUIRE(props.zAir.imag() == Approx(-4506.71377574478));

        REQUIRE(props.ki.real() == Approx(124.345625949829));
        REQUIRE(props.ki.imag() == Approx(-3836.20197910106));

        REQUIRE(props.zaAir.real() == Approx(455.496570353251));
        REQUIRE(props.zaAir.imag() == Approx(-2057.96803661759));
    }

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0}, {22.0, 1.0}, 50.0, 0.0);
        REQUIRE(props.X == Approx(0.0074747434));
        REQUIRE(props.zca.real() == Approx(1355.9444611324));
        REQUIRE(props.zca.imag() == Approx(-1291.26947073563));
        REQUIRE(props.k.real() == Approx(3.65789970046543));
        REQUIRE(props.k.imag() == Approx(-3.17371585732575));
    }

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0, 100.0}, {22.0, 1.0}, 50.0, 15.0);
        REQUIRE(props.X == Approx(0.0074747434));
        REQUIRE(props.zca.real() == Approx(1355.9444611324));
        REQUIRE(props.zca.imag() == Approx(-1291.26947073563));
        REQUIRE(props.k.real() == Approx(3.65789970046543));
        REQUIRE(props.k.imag() == Approx(-3.17371585732575));
    }
}

TEST_CASE("RaumAkustik: detail::hertzToAngular", "[RaumAkustik]")
{
    REQUIRE(mc::detail::hertzToAngular(0.0) == Approx(0.0));
    REQUIRE(mc::detail::hertzToAngular(50.0) == Approx(314.15927));
    REQUIRE(mc::detail::hertzToAngular(53.0) == Approx(333.00882));
    REQUIRE(mc::detail::hertzToAngular(56.0) == Approx(351.85838));
    REQUIRE(mc::detail::hertzToAngular(59.0) == Approx(370.70793));
}

TEST_CASE("RaumAkustik: detail::waveNumber", "[RaumAkustik]")
{
    REQUIRE(mc::detail::waveNumber(20.0, 50.0) == Approx(0.9149));
    REQUIRE(mc::detail::waveNumber(22.0, 50.0) == Approx(0.9118));
}

TEST_CASE("RaumAkustik: detail::delanyBazleyTerm", "[RaumAkustik]")
{
    REQUIRE(mc::detail::delanyBazleyTerm(mc::densityOfAir(20.0, 1.0), 50.0, 8'000.0) == Approx(0.0075257395));
    REQUIRE(mc::detail::delanyBazleyTerm(mc::densityOfAir(22.0, 1.0), 50.0, 8'000.0) == Approx(0.0074747434));
}

TEST_CASE("RaumAkustik: detail::yComponentOfWaveNumber", "[RaumAkustik]")
{
    using namespace mc::detail;
    REQUIRE(yComponentOfWaveNumber(waveNumber(20.0, 50.0), 0.0) == Approx(0.0));
    REQUIRE(yComponentOfWaveNumber(waveNumber(22.0, 50.0), 0.0) == Approx(0.0));

    REQUIRE(yComponentOfWaveNumber(waveNumber(20.0, 50.0), 15.0) == Approx(0.2367929161));
    REQUIRE(yComponentOfWaveNumber(waveNumber(22.0, 50.0), 15.0) == Approx(0.2359892724));
}

TEST_CASE("RaumAkustik: detail::angleOfPropagation", "[RaumAkustik]")
{
    auto const specs = mc::PorousAbsorberSpecs {100.0, 8'000.0};
    REQUIRE(mc::propertiesOfAbsorber(specs, {20.0, 1.0}, 50.0, 0.0).betaPorous == Approx(0.0));
    REQUIRE(mc::propertiesOfAbsorber(specs, {22.0, 1.0}, 50.0, 0.0).betaPorous == Approx(0.0));
    REQUIRE(mc::propertiesOfAbsorber(specs, {20.0, 1.0}, 50.0, 15.0).betaPorous == Approx(2.8036973865));
    REQUIRE(mc::propertiesOfAbsorber(specs, {22.0, 1.0}, 50.0, 15.0).betaPorous == Approx(2.7931256656));
}
