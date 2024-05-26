#pragma once

#include <ra/unit/frequency.hpp>

#include <neo/container/mdspan.hpp>

#include <chrono>
#include <functional>

namespace ra {

struct WaveEquation2D
{
    using Callback = std::function<void(stdex::mdspan<double, stdex::dextents<std::size_t, 2>>)>;

    struct Spec
    {
        quantity<isq::width[si::metre]> Lx;
        quantity<isq::length[si::metre]> Ly;

        quantity<isq::duration[si::second]> duration;
        quantity<isq::frequency[si::hertz]> fmax;
        double ppw{6.0};
    };

    explicit WaveEquation2D(Spec const& spec);

    auto operator()(Callback const& callback) const -> void;

private:
    Spec _spec;
};

}  // namespace ra
