#pragma once

#include <cstddef>
#include <cstdint>

namespace cs2::inventory_offsets
{

// Inventory / loadout offsets — re-verify after game updates.
inline constexpr std::ptrdiff_t kCompositeMaterial = 0x608;
inline constexpr std::ptrdiff_t kSortedItemDefinitionMap = 0x128;
inline constexpr std::ptrdiff_t kPaintKits = 0x2F0;
inline constexpr std::ptrdiff_t kMusicKitDefinitions = 0x500;
inline constexpr std::ptrdiff_t kItemDefinitionLoadoutSlot = 0x338;
inline constexpr std::ptrdiff_t kInventoryManagerLocalInventory = 0x3F540;
inline constexpr std::ptrdiff_t kInventoryManagerLocalInventoryAlt = 0x3D1A0;
inline constexpr std::ptrdiff_t kPlayerInventorySOCache = 0x68;
inline constexpr std::ptrdiff_t kPlayerInventorySOCacheAlt0 = 0x58;
inline constexpr std::ptrdiff_t kPlayerInventorySOCacheAlt1 = 0x70;
inline constexpr std::ptrdiff_t kPlayerInventoryOwner = 0x10;
inline constexpr std::ptrdiff_t kPlayerInventoryItemVector = 0x20;
inline constexpr std::ptrdiff_t kEconItemSystemSchema = 0x8;
inline constexpr std::ptrdiff_t kPaintKitLegacyModel = 0xAE;
inline constexpr std::ptrdiff_t kSOTypeCacheObjects = 0x8;

inline constexpr std::ptrdiff_t kDefIndex = 0x10;
inline constexpr std::ptrdiff_t kItemRarity = 0x42;
inline constexpr std::ptrdiff_t kItemBaseName = 0x70;
inline constexpr std::ptrdiff_t kItemTypeName = 0x80;
inline constexpr std::ptrdiff_t kItemModelName = 0x148;
inline constexpr std::ptrdiff_t kStickerSupportCount = 0x168;

}

namespace cs2
{

enum EEconItemQuality : int {
    IQ_NORMAL = 0,
    IQ_UNUSUAL = 3,
};

enum EEconTypeID : int {
    k_EEconTypeItem = 1,
};

enum EEconItemAttribute : std::uint32_t {
    ATTRIBUTE_PAINT_KIT = 6,
    ATTRIBUTE_PAINT_SEED = 7,
    ATTRIBUTE_PAINT_WEAR = 8,
    ATTRIBUTE_STAT_TRACK = 80,
    ATTRIBUTE_STAT_TRACK_TYPE = 81,
    ATTRIBUTE_STICKER_ID = 113,
    ATTRIBUTE_MUSIC_ID = 166,
};

enum ESOCacheEvent : int {
    eSOCacheEvent_Incremental = 4,
};

struct SOID_t {
    std::uint64_t id{};
    std::uint32_t type{};
    std::uint32_t padding{};
};

}
