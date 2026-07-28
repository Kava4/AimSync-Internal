#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Econ/EconEntityOffsets.h>

template <typename HookContext>
class WeaponEcon {
public:
    explicit WeaponEcon(cs2::C_CSWeaponBase* weapon) noexcept
        : weapon{weapon}
    {
    }

    [[nodiscard]] std::uint16_t itemDefinitionIndex() const noexcept
    {
        if (const auto item = itemViewBytes())
            return *reinterpret_cast<const std::uint16_t*>(item + cs2::econ_offsets::kItemDefinitionIndex);
        return 0;
    }

    [[nodiscard]] std::int32_t fallbackPaintKit() const noexcept
    {
        if (!weapon)
            return 0;
        return *reinterpret_cast<const std::int32_t*>(reinterpret_cast<const std::byte*>(weapon) + cs2::econ_offsets::kFallbackPaintKit);
    }

    void applySkin(std::int32_t paintKit, float wear, std::uint32_t accountId, std::int32_t seed, bool isKnife, std::uint64_t itemId = 0, std::int32_t statTrak = -1) const noexcept
    {
        if (!weapon || paintKit <= 0)
            return;

        auto* const item = itemViewBytes();
        if (!item)
            return;

        auto* const weaponBytes = reinterpret_cast<std::byte*>(weapon);

        item[cs2::econ_offsets::kItemStockItem] = std::byte{0};
        *reinterpret_cast<bool*>(item + cs2::econ_offsets::kItemAttributesDirty) = true;
        *reinterpret_cast<bool*>(item + cs2::econ_offsets::kItemInitialized) = true;

        if (itemId != 0) {
            *reinterpret_cast<std::uint64_t*>(item + cs2::econ_offsets::kItemId) = itemId;
            *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kItemIdHigh) = static_cast<std::uint32_t>(itemId >> 32);
            *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kItemIdLow) = static_cast<std::uint32_t>(itemId);
        } else {
            *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kItemIdHigh) = 0xFFFFFFFF;
            *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kItemIdLow) = 0xFFFFFFFF;
        }

        *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kAccountId) = accountId;
        *reinterpret_cast<std::uint32_t*>(weaponBytes + cs2::econ_offsets::kOriginalOwnerXuidLow) = accountId;
        *reinterpret_cast<std::uint32_t*>(weaponBytes + cs2::econ_offsets::kOriginalOwnerXuidHigh) = 0;

        *reinterpret_cast<std::int32_t*>(weaponBytes + cs2::econ_offsets::kFallbackPaintKit) = paintKit;
        *reinterpret_cast<float*>(weaponBytes + cs2::econ_offsets::kFallbackWear) = wear;
        *reinterpret_cast<std::int32_t*>(weaponBytes + cs2::econ_offsets::kFallbackSeed) = seed;
        *reinterpret_cast<std::int32_t*>(weaponBytes + cs2::econ_offsets::kFallbackStatTrak) = statTrak;
        *reinterpret_cast<std::int32_t*>(item + cs2::econ_offsets::kEntityQuality) = isKnife ? 3 : 0;
    }

private:
    [[nodiscard]] std::byte* itemViewBytes() const noexcept
    {
        if (!weapon)
            return nullptr;
        return reinterpret_cast<std::byte*>(weapon) + cs2::econ_offsets::kAttributeManager + cs2::econ_offsets::kItem;
    }

    cs2::C_CSWeaponBase* weapon;
};
