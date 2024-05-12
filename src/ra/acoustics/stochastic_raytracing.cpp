#include "stochastic_raytracing.hpp"

namespace ra {

StochasticRaytracing::StochasticRaytracing(Room const& room) noexcept : _room{room} {}

auto StochasticRaytracing::operator()(Simulation const& simulation) const -> Result
{
    auto rng = std::mt19937{std::random_device{}()};

    auto const rays         = randomRaysOnSphere(simulation.raysPerFrequency, rng);
    auto const numTimeSteps = static_cast<std::size_t>(simulation.duration / simulation.timeStep);

    auto histogram = std::vector(simulation.frequencies.size(), std::vector<double>(numTimeSteps));
    for (auto frequency{0UL}; frequency < histogram.size(); ++frequency) {
        for (auto const& ray : rays) {
            tarceRay(simulation, ray, histogram[frequency], frequency, rng);
        }
    }

    return histogram;
}

auto StochasticRaytracing::tarceRay(
    Simulation const& sim,
    Vec3 ray,
    std::span<double> histogram,
    std::size_t freqIdx,
    std::mt19937& rng
) const -> void
{
    static constexpr auto const speedOfSound = 343.0;

    auto dist = std::uniform_real_distribution<double>{0.0, 1.0};

    // All rays start at the source/transmitter
    auto rayPos = _room.source;

    // Set initial ray direction. This direction changes as the ray is
    // reflected off surfaces.
    auto rayDir = ray;

    // Initialize ray travel time. Ray tracing is terminated when the
    // travel time exceeds the impulse response length.
    auto rayTime = std::chrono::duration<double>{0.0};

    // Initialize the ray energy to a normalized value of 1. Energy
    // decreases when the ray hits a surface.
    auto rayEnergy = 1.0;

    while (rayTime <= sim.duration) {
        // Determine the surface that the ray encounters
        auto const [surfaceIdx, displacement] = getImpactSurface(rayPos, rayDir);

        auto const surface = static_cast<RoomSurface>(surfaceIdx);

        // Determine the distance traveled by the ray
        auto const distance = std::sqrt(sum(pow(displacement, 2.0)));

        // Determine the coordinates of the impact point
        auto const impactPosition = rayPos + displacement;

        // Update ray location/source
        rayPos = impactPosition;

        // Update cumulative ray travel time
        rayTime = rayTime + std::chrono::duration<double>{distance / speedOfSound};

        // Apply surface reflection to ray's energy
        // This is the amount of energy that is not lost through
        // absorption.
        rayEnergy = rayEnergy * _room.reflection.surface(surface)[freqIdx];  // R[surfaceIdx, freq_idx]

        // Apply diffuse reflection to ray energy
        // This is the fraction of energy used to determine what is
        // detected at the receiver
        auto const recvEnergy = rayEnergy * _room.scattering.surface(surface)[freqIdx];  // D[surfaceIdx, freq_idx]

        // Determine impact point-to-receiver direction.
        auto const recvDir = _room.receiver - impactPosition;

        // Determine the ray's time of arrival at receiver.
        auto const recvDistance  = std::sqrt(sum(recvDir * recvDir));
        auto const timeOfArrival = rayTime + std::chrono::duration<double>{recvDistance / speedOfSound};

        if (timeOfArrival > sim.duration) {
            return;
        }

        // Determine amount of diffuse energy that reaches the receiver. See (5.20) in [2].

        // Compute received energy
        auto const radius       = sim.radius;
        auto const recvDir2     = pow(recvDir, 2.0);
        auto const impactNormal = getWallNormal(surfaceIdx);
        auto const cosTheta     = sum(recvDir * impactNormal) / (sqrt(sum(recvDir2)));
        auto const cosAlpha     = sqrt(sum(recvDir2) - std::pow(radius, 2.0)) / sum(recvDir2);
        auto const energy       = (1 - cosAlpha) * 2 * cosTheta * recvEnergy;

        // Update energy histogram
        auto const timeIdx = static_cast<size_t>(std::max(0, int(timeOfArrival / sim.timeStep + 0.5) - 1));
        histogram[timeIdx] = histogram[timeIdx] + energy;

        // Compute a new direction for the ray.
        // Pick a random direction that is in the hemisphere of the
        // normal to the impact surface.
        auto newDir = normalize(Vec3{dist(rng), dist(rng), dist(rng)});
        if (sum(newDir * impactNormal) < 0) {
            newDir = -newDir;
        }

        // Derive the specular reflection with respect to the incident
        // wall
        auto ref = rayDir - 2.0 * sum(rayDir * impactNormal) * impactNormal;

        // Combine the specular and random components
        auto d = _room.scattering.surface(surface)[freqIdx];  // D[_surface_idx, freq_idx];
        newDir = newDir / norm(newDir);
        ref    = ref / norm(ref);
        rayDir = d * newDir + (1 - d) * ref;
        rayDir = rayDir / norm(rayDir);
    }
}

auto StochasticRaytracing::getImpactSurface(Point pos, Vec3 direction) const -> std::pair<std::ptrdiff_t, Vec3>
{
    auto const length = _room.dimensions.length;
    auto const width  = _room.dimensions.width;
    auto const height = _room.dimensions.height;

    auto surface      = static_cast<ptrdiff_t>(-1);
    auto displacement = 1000.0;

    // Compute time to intersection with x-surfaces
    if (direction.x < 0) {
        displacement = -pos.x / direction.x;
        if (displacement <= 0.0) {
            displacement = 1000;
        }
        surface = 0;
    } else if (direction.x > 0) {
        displacement = (length - pos.x) / direction.x;
        if (displacement <= 0.0) {
            displacement = 1000;
        }
        surface = 1;
    }

    // Compute time to intersection with y-surfaces
    if (direction.y < 0) {
        auto const t = -pos.y / direction.y;
        if (t < displacement and t > 0) {
            surface      = 2;
            displacement = t;
        }
    } else if (direction.y > 0) {
        auto const t = (width - pos.y) / direction.y;
        if (t < displacement and t > 0) {
            surface      = 3;
            displacement = t;
        }
    }

    // Compute time to intersection with z-surfaces
    if (direction.z < 0) {
        auto const t = -pos.z / direction.z;
        if (t < displacement and t > 0) {
            surface      = 4;
            displacement = t;
        }
    } else if (direction.z > 0) {
        auto const t = (height - pos.z) / direction.z;
        if (t < displacement and t > 0) {
            surface      = 5;
            displacement = t;
        }
    }

    assert(surface != -1);
    return {surface, displacement * direction};
}

auto StochasticRaytracing::randomRaysOnSphere(size_t count, std::mt19937& rng) const -> std::vector<Vec3>
{
    auto rays = std::vector<Vec3>(count);
    auto dist = std::uniform_real_distribution<double>{0.0, 1.0};

    for (auto i{0U}; i < count; ++i) {
        // Sample the unfolded right cylinder
        auto z = 2.0 * dist(rng) - 1.0;

        // Convert z to latitude
        z = z < -1 ? -1 : z;
        z = z > 1 ? 1 : z;

        auto lon = 2.0 * std::numbers::pi * dist(rng);
        auto lat = std::acos(z);

        // Convert spherical to rectangular co-ords
        auto const s = std::sin(lat);
        auto const x = std::cos(lon) * s;
        auto const y = std::sin(lon) * s;

        rays[i] = Vec3{x, y, z};
    }

    return rays;
}

auto StochasticRaytracing::getWallNormal(std::ptrdiff_t index) -> Vec3
{
    static constexpr auto surfaces = std::array{
        Vec3{1.0,  0.0,  0.0 },
        Vec3{-1.0, 0.0,  0.0 },
        Vec3{0.0,  1.0,  0.0 },
        Vec3{0.0,  -1.0, 0.0 },
        Vec3{0.0,  0.0,  1.0 },
        Vec3{0.0,  0.0,  -1.0},
    };

    assert(index >= 0);
    assert(std::cmp_less(index, surfaces.size()));
    return surfaces[static_cast<size_t>(index)];
}

}  // namespace ra
