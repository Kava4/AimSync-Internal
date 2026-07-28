#pragma once

#include <algorithm>
#include <cmath>
#include <optional>

#include <CS2/Bones/BoneIds.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/EntitySystem/CEntityIdentity.h>
#include <GameClient/Entities/PlayerBones.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <GameClient/WorldToScreen/WorldToScreen.h>
#include <HookContext/HookContextMacros.h>
#include <ThirdParty/ImGui/imgui.h>

#include "PlayerEspConfigVariables.h"

template <typename HookContext>
class PlayerEsp {
public:
    explicit PlayerEsp(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void draw() const noexcept
    {
        if (!GET_CONFIG_VAR(player_esp_vars::Enabled))
            return;

        if (!GET_CONFIG_VAR(player_esp_vars::ShowBox)
            && !GET_CONFIG_VAR(player_esp_vars::ShowSkeleton)
            && !GET_CONFIG_VAR(player_esp_vars::ShowHeadDot)
            && !GET_CONFIG_VAR(player_esp_vars::ShowHealthBar))
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        if (!drawList)
            return;

        WorldToScreen w2s{hookContext};
        hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([this, drawList, &w2s](const auto& entityIdentity) {
            drawPlayer(entityIdentity, drawList, w2s);
        });
    }

private:
    struct ScreenBone {
        ImVec2 pos{};
        bool visible = false;
    };

    void drawPlayer(const cs2::CEntityIdentity& entityIdentity, ImDrawList* drawList, WorldToScreen<HookContext>& w2s) const noexcept
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

        if (GET_CONFIG_VAR(player_esp_vars::OnlyEnemies) && !playerPawn.isEnemy().value_or(false))
            return;

        if (GET_CONFIG_VAR(player_esp_vars::VisibilityCheck) && !playerPawn.isSpotted().valueOr(false))
            return;

        PlayerBones bones{hookContext, static_cast<cs2::C_CSPlayerPawn*>(entityIdentity.entity)};
        const auto* joints = bones.boneArray();
        if (!joints)
            return;

        ScreenBone screenBones[cs2::kBoneCount]{};
        int visibleCount = 0;

        for (std::size_t i = 0; i < cs2::kBoneCount; ++i) {
            if (const auto screen = w2s.toScreen(joints[i].pos)) {
                screenBones[i] = {*screen, true};
                ++visibleCount;
            }
        }

        if (visibleCount == 0)
            return;

        const ImU32 boxColor = IM_COL32(90, 140, 255, 220);
        const ImU32 boneColor = IM_COL32(200, 205, 215, 220);
        const ImU32 headColor = IM_COL32(200, 205, 215, 220);

        if (GET_CONFIG_VAR(player_esp_vars::ShowSkeleton))
            drawSkeleton(drawList, screenBones, boneColor);

        if (GET_CONFIG_VAR(player_esp_vars::ShowHeadDot) && screenBones[static_cast<std::size_t>(cs2::BoneId::Head)].visible
            && screenBones[static_cast<std::size_t>(cs2::BoneId::Neck)].visible) {
            const auto& head = screenBones[static_cast<std::size_t>(cs2::BoneId::Head)].pos;
            const auto& neck = screenBones[static_cast<std::size_t>(cs2::BoneId::Neck)].pos;
            const float radius = std::abs(head.y - neck.y) + 2.0f;
            drawList->AddCircle(head, radius, headColor, 0, 1.2f);
        }

        const auto box = computeBox(screenBones);
        if (!box)
            return;

        const ImVec2 boxMin{box->x, box->y};
        const ImVec2 boxMax{box->x + box->z, box->y + box->w};

        if (GET_CONFIG_VAR(player_esp_vars::ShowBox)) {
            if (GET_CONFIG_VAR(player_esp_vars::BoxType) == 0)
                drawList->AddRect(boxMin, boxMax, boxColor, 0.0f, 0, 1.3f);
            else
                drawCornerBox(drawList, boxMin, boxMax, boxColor, 1.3f);
        }

        if (GET_CONFIG_VAR(player_esp_vars::ShowHealthBar)) {
            const int hp = (std::clamp)(playerPawn.health().value(), 0, 100);
            const float frac = static_cast<float>(hp) / 100.0f;
            const float barX = boxMin.x - 6.0f;
            drawList->AddRectFilled(ImVec2{barX, boxMin.y}, ImVec2{barX + 3.0f, boxMax.y}, IM_COL32(20, 20, 20, 180));
            drawList->AddRectFilled(
                ImVec2{barX, boxMax.y - (boxMax.y - boxMin.y) * frac},
                ImVec2{barX + 3.0f, boxMax.y},
                IM_COL32(70, 200, 90, 255));
        }
    }

    [[nodiscard]] static std::optional<ImVec4> computeBox(const ScreenBone (&screenBones)[cs2::kBoneCount]) noexcept
    {
        const auto& headBone = screenBones[static_cast<std::size_t>(cs2::BoneId::Head)];
        if (!headBone.visible)
            return std::nullopt;

        float minX = headBone.pos.x;
        float minY = headBone.pos.y;
        float maxX = headBone.pos.x;
        float maxY = headBone.pos.y;

        for (std::size_t i = 0; i < cs2::kBoneCount; ++i) {
            if (!screenBones[i].visible)
                continue;
            minX = (std::min)(minX, screenBones[i].pos.x);
            minY = (std::min)(minY, screenBones[i].pos.y);
            maxX = (std::max)(maxX, screenBones[i].pos.x);
            maxY = (std::max)(maxY, screenBones[i].pos.y);
        }

        const float diffY = maxY - headBone.pos.y;
        const float height = (std::max)(diffY * 1.09f, 8.0f);
        const float width = height * 0.6f;
        const float posX = headBone.pos.x - width * 0.5f;
        const float posY = headBone.pos.y - height * 0.08f;

        minX = (std::min)(minX, posX);
        minY = (std::min)(minY, posY);
        maxX = (std::max)(maxX, posX + width);
        maxY = (std::max)(maxY, posY + height);

        return ImVec4{minX, minY, maxX - minX, maxY - minY};
    }

    static void drawSkeleton(ImDrawList* drawList, const ScreenBone (&screenBones)[cs2::kBoneCount], ImU32 color) noexcept
    {
        drawBoneChain(drawList, screenBones, cs2::bone_chains::kTrunk, color);
        drawBoneChain(drawList, screenBones, cs2::bone_chains::kLeftArm, color);
        drawBoneChain(drawList, screenBones, cs2::bone_chains::kRightArm, color);
        drawBoneChain(drawList, screenBones, cs2::bone_chains::kLeftLeg, color);
        drawBoneChain(drawList, screenBones, cs2::bone_chains::kRightLeg, color);
    }

    template <std::size_t N>
    static void drawBoneChain(ImDrawList* drawList, const ScreenBone (&screenBones)[cs2::kBoneCount], const cs2::BoneId (&chain)[N], ImU32 color) noexcept
    {
        bool hasPrev = false;
        ImVec2 prev{};
        for (const auto bone : chain) {
            const auto& current = screenBones[static_cast<std::size_t>(bone)];
            if (!current.visible) {
                hasPrev = false;
                continue;
            }
            if (hasPrev)
                drawList->AddLine(prev, current.pos, color, 1.3f);
            prev = current.pos;
            hasPrev = true;
        }
    }

    static void drawCornerBox(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImU32 color, float thickness) noexcept
    {
        const float w = max.x - min.x;
        const float h = max.y - min.y;
        const float qw = w * 0.25f;
        const float qh = h * 0.25f;

        const ImVec2 tl = min;
        const ImVec2 tr{max.x, min.y};
        const ImVec2 bl{min.x, max.y};
        const ImVec2 br = max;

        drawList->AddLine(tl, ImVec2{tl.x + qw, tl.y}, color, thickness);
        drawList->AddLine(tl, ImVec2{tl.x, tl.y + qh}, color, thickness);
        drawList->AddLine(tr, ImVec2{tr.x - qw, tr.y}, color, thickness);
        drawList->AddLine(tr, ImVec2{tr.x, tr.y + qh}, color, thickness);
        drawList->AddLine(bl, ImVec2{bl.x + qw, bl.y}, color, thickness);
        drawList->AddLine(bl, ImVec2{bl.x, bl.y - qh}, color, thickness);
        drawList->AddLine(br, ImVec2{br.x - qw, br.y}, color, thickness);
        drawList->AddLine(br, ImVec2{br.x, br.y - qh}, color, thickness);
    }

    HookContext& hookContext;
};
