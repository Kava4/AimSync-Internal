#pragma once

#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Econ/EconEntityOffsets.h>
#include <CS2/Econ/ItemDefinitionIndex.h>
#include <GameClient/Econ/WeaponEcon.h>
#include <GameClient/Entities/BaseEntity.h>
#include <HookContext/HookContextMacros.h>

#include "SkinChangerConfigVariables.h"
#include "SkinChangerState.h"

template <typename HookContext>
class SkinChanger {
public:
    explicit SkinChanger(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() noexcept
    {
        if (!GET_CONFIG_VAR(skin_changer_vars::Enabled))
            return;

        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn)
            return;

        const auto accountId = hookContext.localPlayerController().steamAccountId();
        const auto wear = wearValue();
        auto& changerState = state();

        localPlayerPawn.weapons().forEach([this, accountId, wear, &changerState](const auto& weaponEntity) {
            if (!weaponEntity.classify().isWeapon())
                return;

            auto* const weapon = static_cast<cs2::C_CSWeaponBase*>(static_cast<cs2::C_BaseEntity*>(weaponEntity));
            if (!weapon)
                return;

            WeaponEcon<HookContext> econ{weapon};
            const auto defIndex = econ.itemDefinitionIndex();
            const auto targetPaint = getTargetPaintKit(defIndex);
            if (targetPaint <= 0)
                return;

            if (!changerState.forceFullUpdate && econ.fallbackPaintKit() == targetPaint)
                return;

            changerState.dirtyCounter++;
            const auto seed = static_cast<std::int32_t>((changerState.dirtyCounter * 7919) % 10000 + 1);

            econ.applySkin(targetPaint, wear, accountId, seed, isKnife(defIndex));

            auto* const item = reinterpret_cast<std::byte*>(weapon) + cs2::econ_offsets::kAttributeManager + cs2::econ_offsets::kItem;
            const auto idHigh = changerState.dirtyCounter % 2 == 0 ? 0xFFFFFFFF : 0xFFFFFFFE;
            *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kItemIdHigh) = idHigh;
        });

        changerState.forceFullUpdate = false;
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().skinChangerState;
    }

    [[nodiscard]] float wearValue() const noexcept
    {
        return static_cast<float>(static_cast<std::uint16_t>(GET_CONFIG_VAR(skin_changer_vars::Wear))) / 10000.0f;
    }

    [[nodiscard]] static bool isKnife(std::uint16_t defIndex) noexcept
    {
        return (defIndex >= 500 && defIndex <= 525) || defIndex == 42 || defIndex == 59;
    }

    [[nodiscard]] std::int32_t getTargetPaintKit(std::uint16_t defIndex) const noexcept
    {
        switch (static_cast<cs2::ItemDefinitionIndex>(defIndex)) {
        case cs2::ItemDefinitionIndex::AK47: return static_cast<std::int32_t>(static_cast<std::uint16_t>(GET_CONFIG_VAR(skin_changer_vars::PaintKitAK47)));
        case cs2::ItemDefinitionIndex::AWP: return static_cast<std::int32_t>(static_cast<std::uint16_t>(GET_CONFIG_VAR(skin_changer_vars::PaintKitAWP)));
        case cs2::ItemDefinitionIndex::Deagle: return static_cast<std::int32_t>(static_cast<std::uint16_t>(GET_CONFIG_VAR(skin_changer_vars::PaintKitDeagle)));
        case cs2::ItemDefinitionIndex::M4A4:
        case cs2::ItemDefinitionIndex::M4A1Silencer:
        case cs2::ItemDefinitionIndex::M4A1SilencerAlt:
        case cs2::ItemDefinitionIndex::USPSilencer:
            return static_cast<std::int32_t>(static_cast<std::uint16_t>(GET_CONFIG_VAR(skin_changer_vars::PaintKitM4A1S)));
        default:
            break;
        }

        if (isKnife(defIndex))
            return static_cast<std::int32_t>(static_cast<std::uint16_t>(GET_CONFIG_VAR(skin_changer_vars::PaintKitKnife)));
        return 0;
    }

    HookContext& hookContext;
};
