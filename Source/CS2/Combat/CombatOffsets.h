#pragma once

#include <cstddef>
#include <cstdint>

namespace cs2
{

// Offsets from cs2-dumper output — update when the game updates.
namespace combat_offsets
{

inline constexpr std::ptrdiff_t kCrosshairEntityIndex = 0x33FC;
inline constexpr std::ptrdiff_t kAttackButton = 33970832;
inline constexpr std::ptrdiff_t kViewAngles = 37054280;
inline constexpr std::ptrdiff_t kViewOffset = 0xE70;
inline constexpr std::ptrdiff_t kShotsFired = 0x1C64;
inline constexpr std::ptrdiff_t kAimPunchServices = 0x1490;
inline constexpr std::ptrdiff_t kAimPunchUnpredictableAngle = 0xA4;
inline constexpr std::ptrdiff_t kEntitySpottedState = 0x1C38;
inline constexpr std::ptrdiff_t kSpotted = 0x8;

}

}
