#pragma once

#include <Config/ConfigVariable.h>
#include "NoRecoilParams.h"

namespace no_recoil_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE_RANGE(Strength, no_recoil_params::kStrength);

}
