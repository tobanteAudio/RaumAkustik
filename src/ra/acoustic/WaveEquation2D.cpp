#include "WaveEquation2D.hpp"

#include <neo/algorithm/copy.hpp>

#include <fmt/format.h>
#include <fmt/os.h>

#include <cmath>

namespace ra {

WaveEquation2D::WaveEquation2D(Spec const& spec) : _spec{spec} {}

auto WaveEquation2D::operator()(Callback const& callback) const -> void
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
    auto Kib      = stdex::mdarray<std::uint8_t, stdex::dextents<std::size_t, 2>>{Nx, Ny};

    auto u     = uBuf.to_mdspan();
    auto uPrev = uPrevBuf.to_mdspan();
    auto uNext = uNextBuf.to_mdspan();

    // Source location and initial condition
    u(Nx / 4, Ny / 4)     = 1.0;
    u(Nx / 4 * 3, Ny / 4) = -1.0;

    // Update equation function with Neumann boundary conditions
    auto const delta = std::pow(c * dt / dx, 2.0);

    fmt::println(
        "Wave: {}x{} Nt={} dx={:.1f}mm fs={:.0f}Hz fmax={:.0f}Hz",
        Nx,
        Ny,
        Nt,
        dx * 1'000.0,
        fs,
        _spec.fmax.numerical_value_in(si::hertz)
    );

    for (auto t{0U}; t < Nt; ++t) {
        for (auto x{1U}; x < Nx - 1U; ++x) {
            for (auto y{1U}; y < Ny - 1U; ++y) {
                auto const prev   = uPrev(x, y);
                auto const now    = u(x, y);
                auto const right  = u(x + 1, y);
                auto const left   = u(x - 1, y);
                auto const top    = u(x, y + 1);
                auto const bottom = u(x, y - 1);

                uNext(x, y) = (2 * now - prev + delta * (right - 2 * now + left) + delta * (top - 2 * now + bottom));

                // u0 = in_mask * (0.5*(right + left + top + bottom) - prev);
                // uNext(x, y) = 0.5 * (right + left + top + bottom) - prev;
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
}

}  // namespace ra
