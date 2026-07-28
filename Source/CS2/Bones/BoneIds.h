#pragma once

#include <cstddef>
#include <cstdint>

#include <CS2/Classes/Vector.h>

namespace cs2
{

enum class BoneId : std::uint8_t {
    Pelvis = 0,
    Spine0 = 1,
    Spine1 = 2,
    Spine2 = 3,
    Spine3 = 4,
    Neck = 5,
    Head = 6,
    ArmUpperL = 8,
    ArmLowerL = 9,
    HandL = 10,
    ArmUpperR = 13,
    ArmLowerR = 14,
    HandR = 15,
    LegUpperL = 22,
    LegLowerL = 23,
    AnkleL = 24,
    LegUpperR = 25,
    LegLowerR = 26,
    AnkleR = 27,
};

inline constexpr std::size_t kBoneCount = 30;
inline constexpr std::size_t kBoneStride = 32; // CBoneData: pos(12) + scale(4) + quat(16)

struct BoneJointData {
    Vector pos;
    float scale;
    float rotX;
    float rotY;
    float rotZ;
    float rotW;
};

namespace bone_chains
{

inline constexpr BoneId kTrunk[] = {BoneId::Neck, BoneId::Spine3, BoneId::Spine2, BoneId::Spine1, BoneId::Spine0, BoneId::Pelvis};
inline constexpr BoneId kLeftArm[] = {BoneId::Neck, BoneId::ArmUpperL, BoneId::ArmLowerL, BoneId::HandL};
inline constexpr BoneId kRightArm[] = {BoneId::Neck, BoneId::ArmUpperR, BoneId::ArmLowerR, BoneId::HandR};
inline constexpr BoneId kLeftLeg[] = {BoneId::Pelvis, BoneId::LegUpperL, BoneId::LegLowerL, BoneId::AnkleL};
inline constexpr BoneId kRightLeg[] = {BoneId::Pelvis, BoneId::LegUpperR, BoneId::LegLowerR, BoneId::AnkleR};

}

}
