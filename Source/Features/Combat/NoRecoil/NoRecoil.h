#pragma once

#include <CS2/Combat/CombatOffsets.h>
#include <GameClient/Combat/CombatMath.h>
#include <GameClient/Combat/ViewAngles.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <Platform/AttackInput.h>

#include "NoRecoilConfigVariables.h"

template <typename HookContext>
class NoRecoil {
public:
    explicit NoRecoil(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() noexcept
    {
        if (!GET_CONFIG_VAR(no_recoil_vars::Enabled)) {
            resetState();
            return;
        }

        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn || !localPlayerPawn.isAlive().value_or(false)) {
            resetState();
            return;
        }

        // Gate on attack held — shotsFired can flicker on the render thread mid-spray,
        // but after mouse-up we must stop so punch decay does not shake the view.
        if (!AttackInput{}.isDown()) {
            resetState();
            return;
        }

        auto&& activeWeapon = localPlayerPawn.getActiveWeapon();
        if (!activeWeapon.isRecoilCompensatable()) {
            resetState();
            return;
        }

        const auto shotsFired = localPlayerPawn.shotsFired();
        if (!shotsFired.hasValue() || shotsFired.value() <= 0) {
            resetState();
            return;
        }

        const auto punchOpt = localPlayerPawn.aimPunchAngle();
        if (!punchOpt.hasValue())
            return;

        const float strength = static_cast<float>(GET_CONFIG_VAR(no_recoil_vars::Strength)) / 100.0f;
        // weapon_recoil_scale == 2
        const cs2::QAngle currentPunch{
            .pitch = punchOpt.value().pitch * 2.0f * strength,
            .yaw = punchOpt.value().yaw * 2.0f * strength,
            .roll = 0.0f
        };

        const float punchLenSq = currentPunch.pitch * currentPunch.pitch + currentPunch.yaw * currentPunch.yaw;
        if (punchLenSq < 0.0001f) {
            // Still holding attack but no punch yet (pre-first-bullet) — keep state cold.
            resetState();
            return;
        }

        auto& recoilState = state();
        if (!recoilState.hasPreviousPunch) {
            recoilState.previousPunch = {};
            recoilState.hasPreviousPunch = true;
        }

        const float dPitch = currentPunch.pitch - recoilState.previousPunch.pitch;
        const float dYaw = currentPunch.yaw - recoilState.previousPunch.yaw;
        recoilState.previousPunch = currentPunch;

        if (dPitch * dPitch + dYaw * dYaw < 0.0000001f)
            return;

        ViewAngles viewAngles{};
        const auto currentAngles = viewAngles.get();
        if (!currentAngles.hasValue())
            return;

        viewAngles.set(combat_math::normalizeAngles(cs2::QAngle{
            .pitch = currentAngles.value().pitch - dPitch,
            .yaw = currentAngles.value().yaw - dYaw,
            .roll = 0.0f
        }));
    }

private:
    void resetState() noexcept
    {
        state() = {};
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().noRecoilState;
    }

    HookContext& hookContext;
};
