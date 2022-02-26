#pragma once

#include <cstddef>

namespace mc
{
[[nodiscard]] auto oactaveSubdivision(double startFrequencyHertz, std::size_t octaveSubdivisions, std::size_t idx)
    -> double;
}  // namespace mc