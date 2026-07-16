#pragma once

#include <CS2/Classes/Vector.h>
#include <Utils/ColorUtils.h>
#include <Features/Visuals/GrenadePrediction/GrenadeKind.h>

namespace grenade_prediction_params
{
    constexpr float kSimDt = 1.0f / 64.0f;
    constexpr float kSvGravity = 800.0f;
    constexpr float kStopSpeedSq = 400.0f;
    constexpr float kMolotovSlope = 0.8660254f;
    constexpr int   kMaxTicks = 1154;
    constexpr int   kTicksPerPoint = 2;
    constexpr float kClipPushOff = 0.03125f;
    constexpr float kStopDisplacementSq = 1.42e-14f;
    constexpr int   kMaxBounces = 20;
    constexpr int   kMaxCollisionPasses = 4;
    constexpr float kRemainingFractionEpsilon = 0.001f;

    struct GrenadePhysics {
        float gravityScale;
        float elasticity;
        float friction;
    };

    constexpr GrenadePhysics getGrenadePhysics(cs2::GrenadeKind /*kind*/) {
        return {0.40f, 0.45f, 0.20f};
    }

    constexpr float kBaseThrowVelocity = 750.0f;
    constexpr float kPlayerVelocityScale = 1.25f;
    constexpr float kDefaultEyeHeight = 64.06f;
    constexpr float kViewOffsetMin = 30.0f;
    constexpr float kViewOffsetMax = 70.0f;

    constexpr float kSpawnTraceForward = 22.0f;
    constexpr float kSpawnPullBack = 6.0f;
    constexpr float kThrowZOffsetScale = 12.0f;

    constexpr float kDetonateTimeHeFlash = 1.5f;
    constexpr float kDetonateTimeMolotov = 2.0f;
    constexpr float kDetonateTimeDecoy = 10.0f;
    constexpr float kDetonateTimeSmokeCap = 18.0f;

    constexpr float kDotPanelCreationSize = 4.0f;
    constexpr float kTrajectoryDotSize = 3.0f;
    constexpr float kBounceDotSize = 8.0f;
    constexpr float kEndMarkerSize = 10.0f;

    constexpr auto kEndMarkerHue = color::Hue{30.0f / 360.0f};
}
