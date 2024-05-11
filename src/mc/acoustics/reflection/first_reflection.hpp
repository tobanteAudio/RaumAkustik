#pragma once

#include <array>

#define _USE_MATH_DEFINES  // NOLINT
#include <cmath>

namespace ra
{

struct Point
{
    double x{0};
    double y{0};
    double z{0};
};

using Speaker = Point;

struct RoomDimensions
{
    double length{0};
    double width{0};
    double height{0};
};

struct RoomLayout
{
    RoomDimensions dimensions{};
    std::array<Speaker, 2> speakers{};
    Point listenPosition{};
};

struct FirstReflection
{
    double front{0};
    double left{0};
    double right{0};
};

namespace detail
{
inline auto firstReflectionSideWall(RoomDimensions dimensions, Point listenPosition, Speaker speaker,
                                    double x1) -> double
{
    auto const y  = listenPosition.y - speaker.y;
    auto const x2 = dimensions.width - listenPosition.x;
    return listenPosition.y - (y * x2) / (x1 + x2);
}

inline auto firstReflectionFrontWall(RoomDimensions /*dimensions*/, Point listenPosition, Speaker speaker) -> double
{
    auto const y  = std::abs(speaker.x - listenPosition.x);
    auto const x1 = speaker.y;
    auto const x2 = listenPosition.y;
    return listenPosition.x - (y * x2) / (x1 + x2);
}

}  // namespace detail

template<typename InIt, typename OutIt>
auto firstReflections(InIt f, InIt l, OutIt o, RoomDimensions dimensions, Point listenPosition) -> void
{
    while (f != l)
    {
        *o = FirstReflection{
            detail::firstReflectionFrontWall(dimensions, listenPosition, *f),
            detail::firstReflectionSideWall(dimensions, listenPosition, *f, f->x),
            detail::firstReflectionSideWall(dimensions, listenPosition, *f, dimensions.width - f->x),
        };
        ++f;
        ++o;
    }
}

}  // namespace ra
