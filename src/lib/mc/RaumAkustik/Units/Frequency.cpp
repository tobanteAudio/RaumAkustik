#include "Frequency.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

namespace mc
{
auto oactaveSubdivision(Hertz startFrequency, double numSubdivisions, double index) -> Hertz
{
    return Hertz {std::pow(2.0, std::log2(startFrequency.number()) + index / numSubdivisions)};
}
}  // namespace mc