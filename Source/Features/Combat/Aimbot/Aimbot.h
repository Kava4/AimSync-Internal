#pragma once

#include <cmath>

#include <CS2/Bones/BoneIds.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/EntitySystem/CEntityIdentity.h>
#include <GameClient/Combat/CombatMath.h>
#include <GameClient/Combat/ViewAngles.h>
#include <GameClient/Entities/PlayerBones.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <HookContext/HookContextMacros.h>
#include <ThirdParty/ImGui/imgui.h>
#include <UI/Dx11/Dx11Menu.h>

#include "AimbotConfigVariables.h"
#include <Features/Combat/NoRecoil/NoRecoilConfigVariables.h>

#include <Windows.h>

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

        if (isDx11MenuVisible())
            return;

        if (GET_CONFIG_VAR(aimbot_vars::RequireHoldKey)) {
            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
                return;
        }

        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn || !localPlayerPawn.isAlive().value_or(false))
            return;

        if (GET_CONFIG_VAR(aimbot_vars::DisableWhenFlashed) && localPlayerPawn.isFlashed())
            return;

        if (GET_CONFIG_VAR(aimbot_vars::OnlyWhenScoped) && !localPlayerPawn.isScoped().valueOr(false))
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
        bool hasTarget = false;

        hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([this, &eyePosition, &currentAngles, &bestFov, &bestTargetPosition, &hasTarget, maxFov](const auto& entityIdentity) {
            considerTarget(entityIdentity, eyePosition.value(), currentAngles.value(), maxFov, bestFov, bestTargetPosition, hasTarget);
        });

        if (!hasTarget)
            return;

        auto targetAngles = combat_math::calcAngle(eyePosition.value(), bestTargetPosition);

        if (GET_CONFIG_VAR(aimbot_vars::RecoilControl) && !GET_CONFIG_VAR(no_recoil_vars::Enabled)) {
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

    void drawFov() const noexcept
    {
        if (!GET_CONFIG_VAR(aimbot_vars::Enabled) || !GET_CONFIG_VAR(aimbot_vars::DrawFov))
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        if (!drawList)
            return;

        const ImVec2 display = ImGui::GetIO().DisplaySize;
        const ImVec2 center{display.x * 0.5f, display.y * 0.5f};
        constexpr float kDegToRad = 3.14159265f / 180.0f;
        constexpr float kStaticFov = 90.0f;
        const float aimFov = static_cast<float>(GET_CONFIG_VAR(aimbot_vars::Fov));
        const float staticFovTan = tanf(kStaticFov * kDegToRad * 0.5f);
        const float aimFovTan = tanf(aimFov * kDegToRad * 0.5f);
        const float radius = (aimFovTan / staticFovTan) * (display.x * 0.5f);
        drawList->AddCircle(center, radius, IM_COL32(90, 140, 255, 160), 0, 1.5f);
    }

private:
    [[nodiscard]] static cs2::BoneId boneFromConfig(std::uint8_t hitbox) noexcept
    {
        switch (hitbox) {
        case 1: return cs2::BoneId::Neck;
        case 2: return cs2::BoneId::Spine3;
        case 3: return cs2::BoneId::Pelvis;
        default: return cs2::BoneId::Head;
        }
    }

    void considerTarget(const cs2::CEntityIdentity& entityIdentity, const cs2::Vector& eyePosition, const cs2::QAngle& currentAngles, float maxFov, float& bestFov, cs2::Vector& bestTargetPosition, bool& hasTarget) const noexcept
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

        PlayerBones bones{hookContext, static_cast<cs2::C_CSPlayerPawn*>(entityIdentity.entity)};
        const auto hitboxMode = static_cast<std::uint8_t>(GET_CONFIG_VAR(aimbot_vars::Hitbox));
        bool usedBone = false;

        if (hitboxMode == 4) {
            constexpr cs2::BoneId kCandidates[] = {
                cs2::BoneId::Head, cs2::BoneId::Neck, cs2::BoneId::Spine3, cs2::BoneId::Pelvis
            };
            for (const auto bone : kCandidates) {
                if (const auto bonePos = bones.position(bone); bonePos.hasValue()) {
                    usedBone = true;
                    evaluatePoint(eyePosition, currentAngles, bonePos.value(), maxFov, bestFov, bestTargetPosition, hasTarget);
                }
            }
        } else if (const auto bonePos = bones.position(boneFromConfig(hitboxMode)); bonePos.hasValue()) {
            usedBone = true;
            evaluatePoint(eyePosition, currentAngles, bonePos.value(), maxFov, bestFov, bestTargetPosition, hasTarget);
        }

        if (!usedBone) {
            if (const auto targetEye = playerPawn.eyePosition(); targetEye.hasValue())
                evaluatePoint(eyePosition, currentAngles, targetEye.value(), maxFov, bestFov, bestTargetPosition, hasTarget);
        }
    }

    void evaluatePoint(const cs2::Vector& eyePosition, const cs2::QAngle& currentAngles, const cs2::Vector& target, float maxFov, float& bestFov, cs2::Vector& bestTargetPosition, bool& hasTarget) const noexcept
    {
        const auto aimAngles = combat_math::calcAngle(eyePosition, target);
        const auto fov = combat_math::fovDistance(currentAngles, aimAngles);
        if (fov >= maxFov || fov >= bestFov)
            return;

        bestFov = fov;
        bestTargetPosition = target;
        hasTarget = true;
    }

    HookContext& hookContext;
};
