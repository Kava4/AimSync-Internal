#pragma once

#include <cstddef>

namespace cs2
{

// Offsets from cs2-dumper client_dll.hpp — update when the game updates.
namespace econ_offsets
{

inline constexpr std::ptrdiff_t kAttributeManager = 0x1180;
inline constexpr std::ptrdiff_t kItem = 0x50;

inline constexpr std::ptrdiff_t kFallbackPaintKit = 0x1658;
inline constexpr std::ptrdiff_t kFallbackSeed = 0x165C;
inline constexpr std::ptrdiff_t kFallbackWear = 0x1660;

inline constexpr std::ptrdiff_t kItemDefinitionIndex = 0x1BA;
inline constexpr std::ptrdiff_t kEntityQuality = 0x1BC;
inline constexpr std::ptrdiff_t kItemIdHigh = 0x1D0;
inline constexpr std::ptrdiff_t kItemIdLow = 0x1D4;
inline constexpr std::ptrdiff_t kAccountId = 0x1D8;
inline constexpr std::ptrdiff_t kItemInitialized = 0x1E8;

// Used by fallback skin application (not in public schema).
inline constexpr std::ptrdiff_t kItemAttributesDirty = 0x1B0;
inline constexpr std::ptrdiff_t kItemStockItem = 0x1B5;

inline constexpr std::ptrdiff_t kSteamId = 0x780;

}

}
