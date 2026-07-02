#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace aimbot_params
{

constexpr auto kFov = RangeConstrainedVariableParams<std::uint8_t>{.min = 1, .max = 30, .def = 5};
constexpr auto kSmoothness = RangeConstrainedVariableParams<std::uint8_t>{.min = 1, .max = 50, .def = 5};

}
