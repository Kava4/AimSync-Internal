#pragma once

#include <Config/ConfigVariable.h>
#include "SkinChangerParams.h"

namespace skin_changer_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE_RANGE(PaintKitAK47, skin_changer_params::kPaintKitAK47);
CONFIG_VARIABLE_RANGE(PaintKitM4A1S, skin_changer_params::kPaintKitM4A1S);
CONFIG_VARIABLE_RANGE(PaintKitAWP, skin_changer_params::kPaintKitAWP);
CONFIG_VARIABLE_RANGE(PaintKitDeagle, skin_changer_params::kPaintKitDeagle);
CONFIG_VARIABLE_RANGE(PaintKitKnife, skin_changer_params::kPaintKitKnife);
CONFIG_VARIABLE_RANGE(Wear, skin_changer_params::kWear);

}
