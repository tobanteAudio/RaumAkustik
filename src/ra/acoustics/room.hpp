#pragma once

#include <ra/math/vec3.hpp>

#include <cmath>

namespace ra {

struct RoomDimensions
{
    double length{0};
    double width{0};
    double height{0};
};

struct RoomLayout
{
    RoomDimensions dimensions{};
    Point leftSpeaker{};
    Point rightSpeaker{};
    Point listenPosition{};
};

struct RoomAbsorption
{
    double front;
    double back;
    double left;
    double right;
    double ceiling;
    double floor;
};

struct RoomScattering
{
    double front;
    double back;
    double left;
    double right;
    double ceiling;
    double floor;
};

struct RoomReflection
{
    double front;
    double back;
    double left;
    double right;
    double ceiling;
    double floor;
};

[[nodiscard]] inline auto makeReflection(RoomAbsorption const& room) noexcept -> RoomReflection
{
    return {
        .front   = std::sqrt(room.front),
        .back    = std::sqrt(room.back),
        .left    = std::sqrt(room.left),
        .right   = std::sqrt(room.right),
        .ceiling = std::sqrt(room.ceiling),
        .floor   = std::sqrt(room.floor),
    };
}

}  // namespace ra
