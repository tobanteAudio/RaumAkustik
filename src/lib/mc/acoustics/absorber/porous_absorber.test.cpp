#include "porous_absorber.hpp"

#include <mc/acoustics/Air.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

static constexpr auto C20 = mc::AtmosphericEnvironment{mc::celciusToKelvin(20.0), mc::OneAtmosphere};
static constexpr auto C22 = mc::AtmosphericEnvironment{mc::celciusToKelvin(22.0), mc::OneAtmosphere};

TEST_CASE("RaumAkustik: propertiesOfAbsorber", "[RaumAkustik]")
{

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0, 100.0}, C20, mc::Hertz{50.0}, 0.0);
        REQUIRE(props.X == Catch::Approx(0.0075257395));
        REQUIRE(props.zca.real() == Catch::Approx(1355.71904994104));
        REQUIRE(props.zca.imag() == Catch::Approx(-1289.23418591288));
        REQUIRE(props.k.real() == Catch::Approx(3.65727296960812));
        REQUIRE(props.k.imag() == Catch::Approx(-3.17166651642184));
        REQUIRE(props.ky == Catch::Approx(0));
        REQUIRE(props.kx.real() == Catch::Approx(3.65727296960812));
        REQUIRE(props.kx.imag() == Catch::Approx(-3.17166651642184));

        REQUIRE(props.impedance.intermediateTerm.real() == Catch::Approx(1.4401025724));
        REQUIRE(props.impedance.intermediateTerm.imag() == Catch::Approx(1.4611988329));
        REQUIRE(props.impedance.atSurface.real() == Catch::Approx(124.3456259498));
        REQUIRE(props.impedance.atSurface.imag() == Catch::Approx(-3836.20));

        REQUIRE(props.reflectionFactorNoAirGap.real() == Catch::Approx(+0.970361524973876));
        REQUIRE(props.reflectionFactorNoAirGap.imag() == Catch::Approx(-0.211408253330928));
        REQUIRE(props.absorptionFactorNoAirGap == Catch::Approx(0.0137050613));

        REQUIRE(props.kAirY.real() == Catch::Approx(0.0));
        REQUIRE(props.kAirY.imag() == Catch::Approx(0.0));

        REQUIRE(props.kAirX.real() == Catch::Approx(0.914897572490173));
        REQUIRE(props.kAirX.imag() == Catch::Approx(0.0));

        REQUIRE(props.betaAir == Catch::Approx(0.0));

        REQUIRE(props.kAirRatio.real() == Catch::Approx(1.0));
        REQUIRE(props.kAirRatio.imag() == Catch::Approx(0.0));

        REQUIRE(props.zAir.real() == Catch::Approx(0.0));
        REQUIRE(props.zAir.imag() == Catch::Approx(-4506.71377574478));

        REQUIRE(props.ki.real() == Catch::Approx(124.345625949829));
        REQUIRE(props.ki.imag() == Catch::Approx(-3836.20197910106));

        REQUIRE(props.zaAir.real() == Catch::Approx(455.496570353251));
        REQUIRE(props.zaAir.imag() == Catch::Approx(-2057.96803661759));
    }

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0}, C22, mc::Hertz{50.0}, 0.0);
        REQUIRE(props.X == Catch::Approx(0.0074747434));
        REQUIRE(props.zca.real() == Catch::Approx(1355.9444611324));
        REQUIRE(props.zca.imag() == Catch::Approx(-1291.26947073563));
        REQUIRE(props.k.real() == Catch::Approx(3.65789970046543));
        REQUIRE(props.k.imag() == Catch::Approx(-3.17371585732575));
    }

    {
        auto const props = mc::propertiesOfAbsorber({100.0, 8'000.0, 100.0}, C22, mc::Hertz{50.0}, 15.0);
        REQUIRE(props.X == Catch::Approx(0.0074747434));
        REQUIRE(props.zca.real() == Catch::Approx(1355.9444611324));
        REQUIRE(props.zca.imag() == Catch::Approx(-1291.26947073563));
        REQUIRE(props.k.real() == Catch::Approx(3.65789970046543));
        REQUIRE(props.k.imag() == Catch::Approx(-3.17371585732575));
    }
}

TEST_CASE("RaumAkustik: detail::waveNumber", "[RaumAkustik]")
{
    REQUIRE(mc::detail::waveNumber(mc::celciusToKelvin(20.0), mc::Hertz{50.0}) == Catch::Approx(0.9149));
    REQUIRE(mc::detail::waveNumber(mc::celciusToKelvin(22.0), mc::Hertz{50.0}) == Catch::Approx(0.9118));
}

TEST_CASE("RaumAkustik: detail::delanyBazleyTerm", "[RaumAkustik]")
{
    {
        auto const density = mc::densityOfAir(mc::celciusToKelvin(20.0), mc::OneAtmosphere);
        REQUIRE(mc::detail::delanyBazleyTerm(density, mc::Hertz{50.0}, 8'000.0) == Catch::Approx(0.0075257395));
    }
    {
        auto const density = mc::densityOfAir(mc::celciusToKelvin(22.0), mc::OneAtmosphere);
        REQUIRE(mc::detail::delanyBazleyTerm(density, mc::Hertz{50.0}, 8'000.0) == Catch::Approx(0.0074747434));
    }
}

TEST_CASE("RaumAkustik: detail::yComponentOfWaveNumber", "[RaumAkustik]")
{
    using namespace mc::detail;
    REQUIRE(yComponentOfWaveNumber(waveNumber(mc::celciusToKelvin(20.0), mc::Hertz{50.0}), 0.0) == Catch::Approx(0.0));
    REQUIRE(yComponentOfWaveNumber(waveNumber(mc::celciusToKelvin(22.0), mc::Hertz{50.0}), 0.0) == Catch::Approx(0.0));

    REQUIRE(yComponentOfWaveNumber(waveNumber(mc::celciusToKelvin(20.0), mc::Hertz{50.0}), 15.0)
            == Catch::Approx(0.2367929161));
    REQUIRE(yComponentOfWaveNumber(waveNumber(mc::celciusToKelvin(22.0), mc::Hertz{50.0}), 15.0)
            == Catch::Approx(0.2359892724));
}

TEST_CASE("RaumAkustik: detail::angleOfPropagation", "[RaumAkustik]")
{
    auto const specs = mc::PorousAbsorberSpecs{100.0, 8'000.0};
    REQUIRE(mc::propertiesOfAbsorber(specs, C20, mc::Hertz{50.0}, 0.0).betaPorous == Catch::Approx(0.0));
    REQUIRE(mc::propertiesOfAbsorber(specs, C22, mc::Hertz{50.0}, 0.0).betaPorous == Catch::Approx(0.0));
    REQUIRE(mc::propertiesOfAbsorber(specs, C20, mc::Hertz{50.0}, 15.0).betaPorous == Catch::Approx(2.8036973865));
    REQUIRE(mc::propertiesOfAbsorber(specs, C22, mc::Hertz{50.0}, 15.0).betaPorous == Catch::Approx(2.7931256656));
}
