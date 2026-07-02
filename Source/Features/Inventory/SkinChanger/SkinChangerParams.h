#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace skin_changer_params
{

constexpr auto kPaintKit = RangeConstrainedVariableParams<std::uint16_t>{.min = 0, .max = 2000, .def = 0};
constexpr auto kPaintKitAK47 = RangeConstrainedVariableParams<std::uint16_t>{.min = 0, .max = 2000, .def = 180};
constexpr auto kPaintKitM4A1S = RangeConstrainedVariableParams<std::uint16_t>{.min = 0, .max = 2000, .def = 430};
constexpr auto kPaintKitAWP = RangeConstrainedVariableParams<std::uint16_t>{.min = 0, .max = 2000, .def = 344};
constexpr auto kPaintKitDeagle = RangeConstrainedVariableParams<std::uint16_t>{.min = 0, .max = 2000, .def = 108};
constexpr auto kPaintKitKnife = RangeConstrainedVariableParams<std::uint16_t>{.min = 0, .max = 2000, .def = 568};
constexpr auto kWear = RangeConstrainedVariableParams<std::uint16_t>{.min = 1, .max = 1000, .def = 1};

}
