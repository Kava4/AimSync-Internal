#pragma once

#include <Config/ConfigVariable.h>
#include "AimbotParams.h"

namespace aimbot_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(TeamCheck, bool, true);
CONFIG_VARIABLE(VisibilityCheck, bool, false);
CONFIG_VARIABLE(RecoilControl, bool, true);
CONFIG_VARIABLE_RANGE(Fov, aimbot_params::kFov);
CONFIG_VARIABLE_RANGE(Smoothness, aimbot_params::kSmoothness);

}
