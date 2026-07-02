#pragma once

#include <Config/ConfigVariable.h>
#include "TriggerbotParams.h"

namespace triggerbot_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(TeamCheck, bool, true);
CONFIG_VARIABLE_RANGE(DelayMs, triggerbot_params::kDelayMs);

}
