#pragma once

#include <ra/acoustics/room.hpp>
#include <ra/math/vec3.hpp>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <numbers>
#include <random>
#include <span>
#include <utility>

namespace ra {

struct StochasticRaytracing
{
    struct Room
    {
        RoomDimensions dimensions;
        RoomAbsorption absorption;
        RoomReflection reflection;
        RoomScattering scattering;
        Point source;
        Point receiver;
    };

    struct Simulation
    {
        std::vector<double> frequencies;
        std::size_t rays;
        std::chrono::duration<double> duration;
        std::chrono::duration<double> timeStep;
        double radius;
    };

    using Result = std::vector<std::vector<double>>;

    explicit StochasticRaytracing(Room room) noexcept;

    [[nodiscard]] auto operator()(Simulation const& simulation) const -> Result;

private:
    [[nodiscard]] static auto randomRaysOnSphere(size_t count, std::mt19937& rng) -> std::vector<Vec3>;

    auto tarceRay(Simulation const& sim, Vec3 ray, std::span<double> histogram, std::size_t freqIdx, std::mt19937& rng)
        const -> void;
    [[nodiscard]] auto getImpactSurface(Point pos, Vec3 direction) const -> std::pair<std::ptrdiff_t, Vec3>;
    [[nodiscard]] static auto getWallNormal(std::ptrdiff_t index) -> Vec3;

    Room _room;
};

}  // namespace ra
