#pragma once

#include <cstdint>

#include "InventoryOffsets.h"

namespace cs2
{

struct CEconItemDefinition {
    [[nodiscard]] std::uint16_t defIndex() const noexcept
    {
        return *reinterpret_cast<const std::uint16_t*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kDefIndex);
    }

    [[nodiscard]] std::uint8_t itemRarity() const noexcept
    {
        return *reinterpret_cast<const std::uint8_t*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kItemRarity);
    }

    [[nodiscard]] const char* itemBaseName() const noexcept
    {
        return *reinterpret_cast<const char* const*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kItemBaseName);
    }

    [[nodiscard]] const char* itemTypeName() const noexcept
    {
        return *reinterpret_cast<const char* const*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kItemTypeName);
    }

    [[nodiscard]] const char* modelName() const noexcept
    {
        return *reinterpret_cast<const char* const*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kItemModelName);
    }

    [[nodiscard]] std::int32_t stickerSupportCount() const noexcept
    {
        return *reinterpret_cast<const std::int32_t*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kStickerSupportCount);
    }

    [[nodiscard]] std::uint32_t loadoutSlot() const noexcept
    {
        return *reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kItemDefinitionLoadoutSlot);
    }

    [[nodiscard]] bool isWeapon() const noexcept
    {
        return stickerSupportCount() >= 4;
    }

    [[nodiscard]] bool isKnife(bool excludeDefault) const noexcept
    {
        const char* type = itemTypeName();
        if (!type || !stringsEqual(type, "#CSGO_Type_Knife"))
            return false;
        return excludeDefault ? defIndex() >= 500 : true;
    }

    [[nodiscard]] bool isGlove(bool excludeDefault) const noexcept
    {
        const char* type = itemTypeName();
        if (!type || !stringsEqual(type, "#Type_Hands"))
            return false;
        const bool isDefault = defIndex() == 5028 || defIndex() == 5029;
        return excludeDefault ? !isDefault : true;
    }

    [[nodiscard]] bool isAgent(bool excludeDefault) const noexcept
    {
        const char* type = itemTypeName();
        if (!type || !stringsEqual(type, "#Type_CustomPlayer"))
            return false;
        const bool isDefault = defIndex() == 5036 || defIndex() == 5037;
        return excludeDefault ? !isDefault : true;
    }

private:
    [[nodiscard]] static bool stringsEqual(const char* a, const char* b) noexcept
    {
        if (!a || !b)
            return false;
        while (*a == *b) {
            if (*a == '\0')
                return true;
            ++a;
            ++b;
        }
        return false;
    }
};

struct CPaintKit {
    int nID;
    const char* sName;
    const char* sDescriptionString;
    const char* sDescriptionTag;
    char pad[0x24]{};
    int nRarity;

    [[nodiscard]] bool usesLegacyModel() const noexcept
    {
        return *reinterpret_cast<const std::uint8_t*>(reinterpret_cast<const std::byte*>(this) + inventory_offsets::kPaintKitLegacyModel) != 0;
    }
};

}
