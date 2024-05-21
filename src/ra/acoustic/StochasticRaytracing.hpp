#pragma once

#include <ra/acoustic/Room.hpp>
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
        glm::dvec3 source;
        glm::dvec3 receiver;
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
    [[nodiscard]] static auto randomRaysOnSphere(size_t count, std::mt19937& rng) -> std::vector<glm::dvec3>;

    auto
    tarceRay(Simulation const& sim, glm::dvec3 ray, std::span<double> histogram, std::size_t freqIdx, std::mt19937& rng)
        const -> void;
    [[nodiscard]] auto getImpactSurface(glm::dvec3 pos, glm::dvec3 dir) const -> std::pair<std::ptrdiff_t, glm::dvec3>;
    [[nodiscard]] static auto getWallNormal(std::ptrdiff_t index) -> glm::dvec3;

    Room _room;
};

}  // namespace ra
