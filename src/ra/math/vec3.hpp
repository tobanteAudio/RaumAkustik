#pragma once

#include <cmath>

namespace ra {

struct Vec3
{
    double x;
    double y;
    double z;

    friend constexpr auto operator+(Vec3 const& lhs, Vec3 const& rhs) noexcept -> Vec3
    {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
        };
    }

    friend constexpr auto operator-(Vec3 const& vec) noexcept -> Vec3
    {
        return {
            -vec.x,
            -vec.y,
            -vec.z,
        };
    }

    friend constexpr auto operator-(Vec3 const& lhs, Vec3 const& rhs) noexcept -> Vec3
    {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z,
        };
    }

    friend constexpr auto operator*(Vec3 const& lhs, Vec3 const& rhs) noexcept -> Vec3
    {
        return {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z,
        };
    }

    friend constexpr auto operator*(Vec3 const& lhs, double rhs) noexcept -> Vec3
    {
        return {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs,
        };
    }

    friend constexpr auto operator*(double lhs, Vec3 const& rhs) noexcept -> Vec3 { return rhs * lhs; }

    friend constexpr auto operator/(Vec3 const& lhs, double rhs) noexcept -> Vec3
    {
        return {
            lhs.x / rhs,
            lhs.y / rhs,
            lhs.z / rhs,
        };
    }
};

[[nodiscard]] constexpr auto norm(Vec3 const& vec) noexcept -> double
{
    return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
}

[[nodiscard]] constexpr auto normalize(Vec3 const& vec) noexcept -> Vec3
{
    auto const n = norm(vec);
    return {vec.x / n, vec.y / n, vec.z / n};
}

[[nodiscard]] constexpr auto sum(Vec3 const& vec) noexcept -> double { return vec.x + vec.y + vec.z; }

[[nodiscard]] constexpr auto pow(Vec3 const& vec, double exp) noexcept -> Vec3
{
    using std::pow;
    return {pow(vec.x, exp), pow(vec.y, exp), pow(vec.z, exp)};
}

struct Point
{
    double x{0};
    double y{0};
    double z{0};

    friend constexpr auto operator+(Point const& lhs, Vec3 const& rhs) noexcept -> Point
    {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
        };
    }

    friend constexpr auto operator-(Point const& lhs, Point const& rhs) noexcept -> Vec3
    {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z,
        };
    }
};

}  // namespace ra
