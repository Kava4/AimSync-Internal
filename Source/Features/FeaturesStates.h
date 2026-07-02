#pragma once

#include "Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisState.h"
#include "Combat/Triggerbot/TriggerbotState.h"
#include "Hud/HudFeaturesStates.h"
#include "Inventory/SkinChanger/SkinChangerState.h"
#include "Visuals/VisualFeaturesStates.h"

struct FeaturesStates {
    HudFeaturesStates hudFeaturesStates;
    VisualFeaturesStates visualFeaturesStates;
    NoScopeInaccuracyVisState noScopeInaccuracyVisState;
    TriggerbotState triggerbotState;
    SkinChangerState skinChangerState;
};
