#pragma once

#include <algorithm>

#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

#include <chrono>
#include <vector>

namespace ra {

using namespace mp_units;

struct GlideSweep
{
    enum struct Curve
    {
        Linear,
        Logarithmic,
    };

    quantity<isq::frequency[si::hertz]> from{20.0 * si::hertz};
    quantity<isq::frequency[si::hertz]> to{20'000.0 * si::hertz};
    Curve curve{Curve::Linear};
    quantity<isq::duration[si::second]> duration{1.0 * si::second};
    quantity<isq::frequency[si::hertz]> sampleRate{44'100.0 * si::hertz};
};

[[nodiscard]] auto generate(GlideSweep const& spec) -> std::vector<float>;

}  // namespace ra
