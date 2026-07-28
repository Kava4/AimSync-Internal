#pragma once

#include <Config/ConfigVariable.h>
#include "PlayerEspParams.h"

namespace player_esp_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(OnlyEnemies, bool, true);
CONFIG_VARIABLE(ShowBox, bool, true);
CONFIG_VARIABLE(ShowSkeleton, bool, true);
CONFIG_VARIABLE(ShowHeadDot, bool, true);
CONFIG_VARIABLE(ShowHealthBar, bool, true);
CONFIG_VARIABLE(VisibilityCheck, bool, false);
CONFIG_VARIABLE_RANGE(BoxType, player_esp_params::kBoxType); // 0 = full, 1 = corners

}
