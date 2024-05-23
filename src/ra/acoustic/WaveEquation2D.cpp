#include "WaveEquation2D.hpp"

#include <neo/algorithm/copy.hpp>

#include <fmt/format.h>
#include <fmt/os.h>

#include <cmath>

namespace ra {

WaveEquation2D::WaveEquation2D(Spec const& spec) : _spec{spec} {}

auto WaveEquation2D::operator()(Callback const& callback) -> void
{
    // Speed of sound (m/s)
    static constexpr auto const c = 343.0;

    // Grid spacing
    auto const dx = c / _spec.fmax.numerical_value_in(si::hertz) / _spec.ppw;

    // Time step (CFL condition)
    auto const dt = std::sqrt(0.5) * dx / c;
    auto const fs = 1.0 / dt;

    // Number of time steps
    auto const T  = _spec.duration.numerical_value_in(si::second);
    auto const Nt = static_cast<size_t>(std::ceil(T / dt));

    // Number of grid points
    auto const Nx = static_cast<size_t>(std::ceil(_spec.Lx.numerical_value_in(si::metre) / dx));
    auto const Ny = static_cast<size_t>(std::ceil(_spec.Ly.numerical_value_in(si::metre) / dx));

    // Initialize pressure fields
    auto uBuf     = stdex::mdarray<double, stdex::dextents<std::size_t, 2>>{Nx, Ny};
    auto uPrevBuf = stdex::mdarray<double, stdex::dextents<std::size_t, 2>>{Nx, Ny};
    auto uNextBuf = stdex::mdarray<double, stdex::dextents<std::size_t, 2>>{Nx, Ny};

    auto u     = uBuf.to_mdspan();
    auto uPrev = uPrevBuf.to_mdspan();
    auto uNext = uNextBuf.to_mdspan();

    // Source location and initial condition
    u(Nx / 2, Ny / 2) = 1.0;

    // Update equation function with Neumann boundary conditions
    auto const delta = std::pow(c * dt / dx, 2.0);

    fmt::println("Wave: {}x{} Nt={} dx={} fs={} fmax={}", Nx, Ny, Nt, dx, fs, _spec.fmax.numerical_value_in(si::hertz));

    for (auto t{1U}; t < Nt - 1U; ++t) {
        for (auto x{1U}; x < Nx - 1U; ++x) {
            for (auto y{1U}; y < Ny - 1U; ++y) {
                uNext(x, y)
                    = (2 * u(x, y) - uPrev(x, y) + delta * (u(x + 1, y) - 2 * u(x, y) + u(x - 1, y))
                       + delta * (u(x, y + 1) - 2 * u(x, y) + u(x, y - 1)));
            }
        }

        // Neumann boundary conditions (rigid walls)
        auto const leftEdge   = stdex::submdspan(uNext, 0, stdex::full_extent);
        auto const leftInside = stdex::submdspan(uNext, 1, stdex::full_extent);
        neo::copy(leftInside, leftEdge);

        auto const rightEdge   = stdex::submdspan(uNext, uNext.extent(0) - 1, stdex::full_extent);
        auto const rightInside = stdex::submdspan(uNext, uNext.extent(0) - 2, stdex::full_extent);
        neo::copy(rightInside, rightEdge);

        auto const bottomEdge   = stdex::submdspan(uNext, stdex::full_extent, 0);
        auto const bottomInside = stdex::submdspan(uNext, stdex::full_extent, 1);
        neo::copy(bottomInside, bottomEdge);

        auto const topEdge   = stdex::submdspan(uNext, stdex::full_extent, uNext.extent(1) - 1);
        auto const topInside = stdex::submdspan(uNext, stdex::full_extent, uNext.extent(1) - 2);
        neo::copy(topInside, topEdge);

        neo::copy(u, uPrev);
        neo::copy(uNext, u);

        if (callback) {
            callback(u);
        }
    }

    return;
}

}  // namespace ra
