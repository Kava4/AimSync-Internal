#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace aimbot_params
{

constexpr auto kFov = RangeConstrainedVariableParams<std::uint8_t>{.min = 1, .max = 45, .def = 10};
constexpr auto kSmoothness = RangeConstrainedVariableParams<std::uint8_t>{.min = 1, .max = 50, .def = 5};
// 0=Head 1=Neck 2=Chest 3=Pelvis 4=Nearest
constexpr auto kHitbox = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = 4, .def = 0};

}
