#include "FirstReflection.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("RaumAkustik: firstReflections", "")
{
    static constexpr auto Room = ra::RoomLayout{
        .dimensions     = ra::RoomDimensions{600, 365, 312},
        .speakers       = {glm::dvec3(121, 160, 125), glm::dvec3(121 + 123, 160, 125)},
        .listenPosition = glm::dvec3(121 + 123 / 2.0, 160 + 123, 120),
    };

    auto reflections = std::array<ra::FirstReflection, 2>{};
    ra::firstReflections(
        begin(Room.speakers),
        end(Room.speakers),
        begin(reflections),
        Room.dimensions,
        Room.listenPosition
    );

    REQUIRE(static_cast<int>(reflections[0].front) == 143);
    REQUIRE(static_cast<int>(reflections[0].left) == 209);
    REQUIRE(static_cast<int>(reflections[0].right) == 230);

    REQUIRE(static_cast<int>(reflections[1].front) == 143);
    REQUIRE(static_cast<int>(reflections[1].right) == 209);
    REQUIRE(static_cast<int>(reflections[1].left) == 230);
}
