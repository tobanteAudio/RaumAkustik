#pragma once

#include <units/isq/si/speed.h>

namespace mc
{
using namespace units::isq;

template<typename T>
using MetrePerSecond = si::speed<si::metre_per_second, T>;
}  // namespace mc