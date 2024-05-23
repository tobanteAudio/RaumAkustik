#pragma once

#include <glm/glm.hpp>

#include <cmath>

namespace ra {

[[nodiscard]] constexpr auto sum(glm::dvec3 const& vec) noexcept -> double { return vec.x + vec.y + vec.z; }

[[nodiscard]] constexpr auto norm(glm::dvec3 const& vec) noexcept -> double
{
    return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
}

[[nodiscard]] constexpr auto pow(glm::dvec3 const& vec, double exp) noexcept -> glm::dvec3
{
    return {std::pow(vec.x, exp), std::pow(vec.y, exp), std::pow(vec.z, exp)};
}

}  // namespace ra
