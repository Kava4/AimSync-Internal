#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace thirdperson_params
{

constexpr auto kDistance = RangeConstrainedVariableParams<std::uint8_t>{.min = 50, .max = 200, .def = 120};

}
