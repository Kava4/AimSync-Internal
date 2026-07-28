#pragma once

#include <Config/ConfigVariable.h>
#include "InventoryChangerParams.h"

namespace inventory_changer_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE_RANGE(AkSkin, inventory_changer_params::kAkSkinIndex);
CONFIG_VARIABLE_RANGE(M4Skin, inventory_changer_params::kM4SkinIndex);
CONFIG_VARIABLE_RANGE(AwpSkin, inventory_changer_params::kAwpSkinIndex);
CONFIG_VARIABLE_RANGE(DeagleSkin, inventory_changer_params::kDeagleSkinIndex);
CONFIG_VARIABLE_RANGE(KnifeSkin, inventory_changer_params::kKnifeSkinIndex);
CONFIG_VARIABLE_RANGE(KnifeType, inventory_changer_params::kKnifeTypeIndex);
CONFIG_VARIABLE_RANGE(Wear, inventory_changer_params::kWear);
CONFIG_VARIABLE_RANGE(Seed, inventory_changer_params::kSeed);
CONFIG_VARIABLE(StatTrak, bool, false);
CONFIG_VARIABLE(ApplyRequested, bool, false);

}
