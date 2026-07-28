#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace player_esp_params
{

constexpr auto kBoxType = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = 1, .def = 1};

}
