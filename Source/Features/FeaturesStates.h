#pragma once

#include "Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisState.h"
#include "Combat/NoRecoil/NoRecoilState.h"
#include "Combat/Triggerbot/TriggerbotState.h"
#include "Inventory/InventoryChanger/InventoryChangerState.h"
#include "Inventory/SkinChanger/SkinChangerState.h"
#include "Hud/HudFeaturesStates.h"
#include "Visuals/VisualFeaturesStates.h"

struct FeaturesStates {
    HudFeaturesStates hudFeaturesStates;
    VisualFeaturesStates visualFeaturesStates;
    NoScopeInaccuracyVisState noScopeInaccuracyVisState;
    NoRecoilState noRecoilState;
    TriggerbotState triggerbotState;
    InventoryChangerState inventoryChangerState;
    SkinChangerState skinChangerState;
};
