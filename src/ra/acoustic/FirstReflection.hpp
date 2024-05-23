#pragma once

#include <ra/acoustic/Room.hpp>
#include <ra/geometry/Vec3.hpp>

#define _USE_MATH_DEFINES  // NOLINT
#include <array>
#include <cmath>

namespace ra {

struct FirstReflection
{
    double front{0};
    double left{0};
    double right{0};
};

namespace detail {
inline auto firstReflectionSideWall(RoomDimensions room, glm::dvec3 receiver, glm::dvec3 source, double x1) -> double
{
    auto const y  = receiver.y - source.y;
    auto const x2 = room.width.numerical_value_in(si::centi<si::metre>) - receiver.x;
    return receiver.y - (y * x2) / (x1 + x2);
}

inline auto firstReflectionFrontWall(RoomDimensions /*room*/, glm::dvec3 receiver, glm::dvec3 source) -> double
{
    auto const y  = std::abs(source.x - receiver.x);
    auto const x1 = source.y;
    auto const x2 = receiver.y;
    return receiver.x - (y * x2) / (x1 + x2);
}

}  // namespace detail

template<typename InIt, typename OutIt>
auto firstReflections(InIt f, InIt l, OutIt o, RoomDimensions room, glm::dvec3 receiver) -> void
{
    while (f != l) {
        *o = FirstReflection{
            detail::firstReflectionFrontWall(room, receiver, *f),
            detail::firstReflectionSideWall(room, receiver, *f, f->x),
            detail::firstReflectionSideWall(
                room,
                receiver,
                *f,
                room.width.numerical_value_in(si::centi<si::metre>) - f->x
            ),
        };
        ++f;
        ++o;
    }
}

}  // namespace ra
