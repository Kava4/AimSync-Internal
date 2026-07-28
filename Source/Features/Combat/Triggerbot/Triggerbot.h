#pragma once

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <Platform/AttackInput.h>

#include "TriggerbotConfigVariables.h"

template <typename HookContext>
class Triggerbot {
public:
    explicit Triggerbot(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() noexcept
    {
        if (!enabled())
            return;

        if (!isValidTarget()) {
            stopForcedAttack();
            return;
        }

        const auto curtime = hookContext.globalVars().curtime();
        if (!curtime.hasValue())
            return;

        auto& triggerState = state();
        if (!triggerState.waitingToFire) {
            triggerState.waitingToFire = true;
            triggerState.fireTime = curtime.value() + static_cast<float>(GET_CONFIG_VAR(triggerbot_vars::DelayMs)) / 1000.0f;
        }

        if (curtime.value() >= triggerState.fireTime) {
            attackInput().press();
            triggerState.forcedAttack = true;
            triggerState.justFired = true;
            triggerState.waitingToFire = false;
            triggerState.holdFramesLeft = 4;
        }

        finishForcedAttackHold();
    }

    void onDisable() noexcept
    {
        stopForcedAttack();
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return GET_CONFIG_VAR(triggerbot_vars::Enabled);
    }

    [[nodiscard]] bool isValidTarget() const noexcept
    {
        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn || !localPlayerPawn.isAlive().value_or(false))
            return false;

        if (GET_CONFIG_VAR(triggerbot_vars::DisableWhenFlashed) && localPlayerPawn.isFlashed())
            return false;

        if (GET_CONFIG_VAR(triggerbot_vars::OnlyWhenScoped) && !localPlayerPawn.isScoped().valueOr(false))
            return false;

        const auto crosshairEntityIndex = localPlayerPawn.crosshairEntityIndex();
        if (!crosshairEntityIndex.has_value() || !crosshairEntityIndex->isValid() || crosshairEntityIndex->value == 0)
            return false;

        auto* targetEntity = hookContext.template make<EntitySystem>().getEntityFromIndex(*crosshairEntityIndex);
        if (!targetEntity)
            return false;

        auto&& targetPawn = hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(targetEntity));
        if (!targetPawn || !targetPawn.isTTorCT())
            return false;

        if (!targetPawn.isAlive().value_or(false))
            return false;

        if (const auto health = targetPawn.health(); !health.hasValue() || health.value() <= 0)
            return false;

        if (targetPawn.hasImmunity().valueOr(false))
            return false;

        if (GET_CONFIG_VAR(triggerbot_vars::TeamCheck) && !targetPawn.isEnemy().value_or(false))
            return false;

        return true;
    }

    void finishForcedAttackHold() noexcept
    {
        auto& triggerState = state();
        if (!triggerState.forcedAttack)
            return;

        if (triggerState.justFired && triggerState.holdFramesLeft > 0) {
            attackInput().press();
            --triggerState.holdFramesLeft;
            return;
        }

        if (triggerState.justFired) {
            attackInput().release();
            triggerState.justFired = false;
            triggerState.forcedAttack = false;
        }
    }

    void stopForcedAttack() noexcept
    {
        auto& triggerState = state();
        if (triggerState.forcedAttack)
            attackInput().release();
        triggerState = {};
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().triggerbotState;
    }

    [[nodiscard]] static AttackInput attackInput() noexcept
    {
        return {};
    }

    HookContext& hookContext;
};
