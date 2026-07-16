#pragma once

#include <CS2/Classes/Color.h>
#include <Config/ConfigVariable.h>
#include <Config/RangeConstrainedVariableParams.h>
#include <cstdint>

namespace grenade_prediction_vars
{
    CONFIG_VARIABLE(Enabled, bool, false);
    constexpr HueVariableParams kTrajectoryHue{.min = 0, .max = 359, .def = 0};
    constexpr HueVariableParams kBounceHue{.min = 0, .max = 359, .def = 120};

    CONFIG_VARIABLE_HUE(TrajectoryHue, kTrajectoryHue);
    CONFIG_VARIABLE_HUE(BounceHue, kBounceHue);

    constexpr RangeConstrainedVariableParams<std::uint8_t> kBounceFriction{.min = 0, .max = 200, .def = 0};
    CONFIG_VARIABLE_RANGE(BounceFriction, kBounceFriction);
}
