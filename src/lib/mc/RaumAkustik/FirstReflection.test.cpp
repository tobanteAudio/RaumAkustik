#include "FirstReflection.hpp"

#include <catch2/catch.hpp>

static constexpr auto Room = mc::RoomLayout{
    mc::RoomDimensions{
        600,
        365,
        312,
    },
    {
        mc::Speaker{
            121,
            160,
            125,
        },
        mc::Speaker{
            121 + 123,
            160,
            125,
        },
    },
    mc::Point{
        121 + 123 / 2.0,
        160 + 123,
        120,
    },
};

TEST_CASE("RaumAkustik: firstReflections", "[RaumAkustik]")
{
    auto reflections = std::array<mc::FirstReflection, 2>{};
    mc::firstReflections(begin(Room.speakers), end(Room.speakers), begin(reflections), Room.dimensions,
                         Room.listenPosition);

    REQUIRE(static_cast<int>(reflections[0].front) == 143);
    REQUIRE(static_cast<int>(reflections[0].left) == 209);
    REQUIRE(static_cast<int>(reflections[0].right) == 230);

    REQUIRE(static_cast<int>(reflections[1].front) == 143);
    REQUIRE(static_cast<int>(reflections[1].right) == 209);
    REQUIRE(static_cast<int>(reflections[1].left) == 230);
}