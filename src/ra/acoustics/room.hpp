#pragma once

#include <ra/math/vec3.hpp>

#include <cmath>
#include <span>
#include <vector>

namespace ra {

enum struct RoomSurface
{
    front,
    back,
    left,
    right,
    ceiling,
    floor,
};

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
    std::vector<double> front;
    std::vector<double> back;
    std::vector<double> left;
    std::vector<double> right;
    std::vector<double> ceiling;
    std::vector<double> floor;

    [[nodiscard]] auto surface(RoomSurface s) const -> std::span<double const>
    {
        switch (s) {
            case RoomSurface::front: return front;
            case RoomSurface::back: return back;
            case RoomSurface::left: return left;
            case RoomSurface::right: return right;
            case RoomSurface::ceiling: return ceiling;
            case RoomSurface::floor: return floor;
            default: break;
        }

        return {};
    }
};

struct RoomScattering
{
    std::vector<double> front;
    std::vector<double> back;
    std::vector<double> left;
    std::vector<double> right;
    std::vector<double> ceiling;
    std::vector<double> floor;

    [[nodiscard]] auto surface(RoomSurface s) const -> std::span<double const>
    {
        switch (s) {
            case RoomSurface::front: return front;
            case RoomSurface::back: return back;
            case RoomSurface::left: return left;
            case RoomSurface::right: return right;
            case RoomSurface::ceiling: return ceiling;
            case RoomSurface::floor: return floor;
            default: break;
        }

        return {};
    }
};

struct RoomReflection
{
    std::vector<double> front;
    std::vector<double> back;
    std::vector<double> left;
    std::vector<double> right;
    std::vector<double> ceiling;
    std::vector<double> floor;

    [[nodiscard]] auto surface(RoomSurface s) const -> std::span<double const>
    {
        switch (s) {
            case RoomSurface::front: return front;
            case RoomSurface::back: return back;
            case RoomSurface::left: return left;
            case RoomSurface::right: return right;
            case RoomSurface::ceiling: return ceiling;
            case RoomSurface::floor: return floor;
            default: break;
        }

        return {};
    }
};

[[nodiscard]] inline auto makeReflection(RoomAbsorption const& room) noexcept -> RoomReflection
{
    auto squareRoot = [](auto const& vec) {
        auto result = std::vector<double>(vec.size());
        std::transform(vec.begin(), vec.end(), result.begin(), [](auto x) { return std::sqrt(1.0 - x); });
        return result;
    };
    return {
        .front   = squareRoot(room.front),
        .back    = squareRoot(room.back),
        .left    = squareRoot(room.left),
        .right   = squareRoot(room.right),
        .ceiling = squareRoot(room.ceiling),
        .floor   = squareRoot(room.floor),
    };
}

}  // namespace ra
