#pragma once

#include <chrono>
#include <vector>

namespace mc
{

struct SineSweepSpec
{
    float start{20.0F};
    float end{20'000.0F};
    std::chrono::milliseconds length{1'000};
    double sampleRate{44'100.0};
};

[[nodiscard]] auto makeSineSweep(SineSweepSpec const& spec) -> std::vector<float>;

}  // namespace mc
