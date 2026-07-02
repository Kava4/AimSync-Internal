#pragma once

#include <cmath>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <GameClient/Combat/CombatMath.h>
#include <GameClient/Combat/ViewAngles.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <CS2/Classes/EntitySystem/CEntityIdentity.h>

#include "AimbotConfigVariables.h"

template <typename HookContext>
class Aimbot {
public:
    explicit Aimbot(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() const noexcept
    {
        if (!GET_CONFIG_VAR(aimbot_vars::Enabled))
            return;

        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn || !localPlayerPawn.isAlive().value_or(false))
            return;

        const auto eyePosition = localPlayerPawn.eyePosition();
        if (!eyePosition.hasValue())
            return;

        ViewAngles viewAngles{};
        const auto currentAngles = viewAngles.get();
        if (!currentAngles.hasValue())
            return;

        const auto maxFov = static_cast<float>(GET_CONFIG_VAR(aimbot_vars::Fov));
        float bestFov = maxFov;
        cs2::Vector bestTargetPosition{};

        hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([this, &eyePosition, &currentAngles, &bestFov, &bestTargetPosition](const auto& entityIdentity) {
            considerTarget(entityIdentity, eyePosition.value(), currentAngles.value(), bestFov, bestTargetPosition);
        });

        if (bestFov >= maxFov)
            return;

        auto targetAngles = combat_math::calcAngle(eyePosition.value(), bestTargetPosition);

        if (GET_CONFIG_VAR(aimbot_vars::RecoilControl)) {
            if (const auto shotsFired = localPlayerPawn.shotsFired(); shotsFired.hasValue() && shotsFired.value() > 0) {
                if (const auto punch = localPlayerPawn.aimPunchAngle(); punch.hasValue()) {
                    targetAngles.pitch -= punch.value().pitch * 2.0f;
                    targetAngles.yaw -= punch.value().yaw * 2.0f;
                }
            }
        }

        targetAngles = combat_math::normalizeAngles(targetAngles);
        const auto smoothed = combat_math::smoothAngles(currentAngles.value(), targetAngles, static_cast<float>(GET_CONFIG_VAR(aimbot_vars::Smoothness)));
        viewAngles.set(smoothed);
    }

private:
    void considerTarget(const cs2::CEntityIdentity& entityIdentity, const cs2::Vector& eyePosition, const cs2::QAngle& currentAngles, float& bestFov, cs2::Vector& bestTargetPosition) const noexcept
    {
        const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(entityIdentity.entityClass);
        if (!entityTypeInfo.template is<cs2::C_CSPlayerPawn>())
            return;

        auto&& playerPawn = hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(entityIdentity.entity));
        if (!playerPawn || playerPawn.isControlledByLocalPlayer())
            return;

        if (!playerPawn.isTTorCT() || !playerPawn.isAlive().value_or(false))
            return;

        if (const auto health = playerPawn.health(); !health.hasValue() || health.value() <= 0)
            return;

        if (playerPawn.hasImmunity().valueOr(false))
            return;

        if (GET_CONFIG_VAR(aimbot_vars::TeamCheck) && !playerPawn.isEnemy().value_or(false))
            return;

        if (GET_CONFIG_VAR(aimbot_vars::VisibilityCheck) && !playerPawn.isSpotted().valueOr(false))
            return;

        const auto targetEyePosition = playerPawn.eyePosition();
        if (!targetEyePosition.hasValue())
            return;

        const auto aimAngles = combat_math::calcAngle(eyePosition, targetEyePosition.value());
        const auto fov = combat_math::fovDistance(currentAngles, aimAngles);
        if (fov < bestFov) {
            bestFov = fov;
            bestTargetPosition = targetEyePosition.value();
        }
    }

    HookContext& hookContext;
};
