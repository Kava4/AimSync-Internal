#pragma once

#include <Config/ConfigVariable.h>
#include "ThirdPersonParams.h"

namespace thirdperson_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE_RANGE(Distance, thirdperson_params::kDistance);

}
