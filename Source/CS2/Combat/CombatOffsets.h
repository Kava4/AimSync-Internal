#pragma once

#include <cstddef>
#include <cstdint>

namespace cs2
{

// Offsets from https://github.com/a2x/cs2-dumper (2026-07-21) — update after game patches.
namespace combat_offsets
{

inline constexpr std::ptrdiff_t kCSGOInput = 0x23BA790; // dwCSGOInput (reserved for future CreateMove work)
inline constexpr std::ptrdiff_t kViewAngles = 0x23BAE18; // dwViewAngles — standalone client global

inline constexpr std::ptrdiff_t kAttackButton = 0x2094F80; // buttons::attack
inline constexpr std::ptrdiff_t kJumpButton = 0x2095490; // buttons::jump
inline constexpr std::ptrdiff_t kLeftButton = 0x20952E0; // buttons::left
inline constexpr std::ptrdiff_t kRightButton = 0x2095370; // buttons::right
inline constexpr std::ptrdiff_t kForwardButton = 0x20951C0; // buttons::forward
inline constexpr std::ptrdiff_t kBackButton = 0x2095250; // buttons::back

inline constexpr std::ptrdiff_t kCrosshairEntityIndex = 0x341C; // C_CSPlayerPawn::m_iIDEntIndex
inline constexpr std::ptrdiff_t kViewOffset = 0xE78; // C_BaseModelEntity::m_vecViewOffset
inline constexpr std::ptrdiff_t kShotsFired = 0x1C84; // C_CSPlayerPawn::m_iShotsFired
inline constexpr std::ptrdiff_t kAimPunchServices = 0x14B8; // C_CSPlayerPawn::m_pAimPunchServices
inline constexpr std::ptrdiff_t kAimPunchPredictableAngle = 0x50; // CCSPlayer_AimPunchServices::m_predictableBaseAngle
inline constexpr std::ptrdiff_t kAimPunchPredictableAngleVel = 0x5C; // CCSPlayer_AimPunchServices::m_predictableBaseAngleVel
inline constexpr std::ptrdiff_t kAimPunchCache = 0x88; // CUtlVector — m_unpredictableBaseTick - 0x18
inline constexpr std::ptrdiff_t kAimPunchUnpredictableAngle = 0xA4; // CCSPlayer_AimPunchServices::m_unpredictableBaseAngle
inline constexpr std::ptrdiff_t kAimPunchUnpredictableTick = 0xA0; // CCSPlayer_AimPunchServices::m_unpredictableBaseTick
inline constexpr std::ptrdiff_t kEntitySpottedState = 0x1C58; // C_CSPlayerPawn::m_entitySpottedState
inline constexpr std::ptrdiff_t kSpotted = 0x8; // EntitySpottedState_t::m_bSpotted
inline constexpr std::ptrdiff_t kFlags = 0x3F4; // C_BaseEntity::m_fFlags
inline constexpr std::uint32_t kFlagOnGround = 1;

inline constexpr std::ptrdiff_t kWeaponTurningInaccuracy = 0x17EC; // C_CSWeaponBase::m_flTurningInaccuracy
inline constexpr std::ptrdiff_t kWeaponAccuracyPenalty = 0x17F0; // C_CSWeaponBase::m_fAccuracyPenalty
inline constexpr std::ptrdiff_t kWeaponAccuracySmoothedForZoom = 0x17F8; // C_CSWeaponBase::m_fAccuracySmoothedForZoom

// CSkeletonInstance::m_modelState (0x140) + CModelState::m_skeletonInstance / bone ptr (0x80)
inline constexpr std::ptrdiff_t kModelState = 0x140;
inline constexpr std::ptrdiff_t kBoneArray = kModelState + 0x80; // 0x1C0 — GameSceneNode + BoneArray*

}

}
