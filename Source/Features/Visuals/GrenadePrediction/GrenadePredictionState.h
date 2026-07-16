#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <CS2/Panorama/PanelHandle.h>
#include <Features/Visuals/GrenadePrediction/Trajectory.h>

struct GrenadePredictionState {
    cs2::PanelHandle containerPanelHandle{};
    Trajectory cachedTrajectory{};
    Trajectory tempTrajectory{};

    float cachedThrowStrength{1.0f};
    const void* cachedThrowStrengthWeapon{};
};
