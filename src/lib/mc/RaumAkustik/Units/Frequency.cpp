#include "Frequency.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

namespace mc
{
auto oactaveSubdivision(double startFrequencyHertz, std::size_t octaveSubdivisions, std::size_t idx) -> double
{
    return std::pow(2.0, std::log2(startFrequencyHertz)
                             + static_cast<double>(idx) / static_cast<double>(octaveSubdivisions));
}
}  // namespace mc