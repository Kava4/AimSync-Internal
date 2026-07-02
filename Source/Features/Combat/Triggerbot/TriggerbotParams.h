#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace triggerbot_params
{

constexpr auto kDelayMs = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = 250, .def = 0};

}
