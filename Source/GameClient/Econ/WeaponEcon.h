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

    void applySkin(std::int32_t paintKit, float wear, std::uint32_t accountId, std::int32_t seed, bool isKnife) const noexcept
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

        *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kItemIdHigh) = 0xFFFFFFFF;
        *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kItemIdLow) = 0xFFFFFFFF;
        *reinterpret_cast<std::uint32_t*>(item + cs2::econ_offsets::kAccountId) = accountId;

        *reinterpret_cast<std::int32_t*>(weaponBytes + cs2::econ_offsets::kFallbackPaintKit) = paintKit;
        *reinterpret_cast<float*>(weaponBytes + cs2::econ_offsets::kFallbackWear) = wear;
        *reinterpret_cast<std::int32_t*>(weaponBytes + cs2::econ_offsets::kFallbackSeed) = seed;
        *reinterpret_cast<std::int32_t*>(item + cs2::econ_offsets::kEntityQuality) = isKnife ? 4 : 0;
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
