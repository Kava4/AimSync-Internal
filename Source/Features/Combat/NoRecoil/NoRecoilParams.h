#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace no_recoil_params
{

constexpr auto kStrength = RangeConstrainedVariableParams<std::uint8_t>{.min = 1, .max = 100, .def = 100};

}
