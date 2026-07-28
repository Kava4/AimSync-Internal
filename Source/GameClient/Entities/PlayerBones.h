#pragma once

#include <CS2/Bones/BoneIds.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Combat/CombatOffsets.h>
#include <MemoryPatterns/PatternTypes/EntityPatternTypes.h>
#include <Utils/Optional.h>

template <typename HookContext>
class PlayerBones {
public:
    PlayerBones(HookContext& hookContext, cs2::C_CSPlayerPawn* playerPawn) noexcept
        : hookContext{hookContext}
        , playerPawn{playerPawn}
    {
    }

    [[nodiscard]] Optional<cs2::Vector> position(cs2::BoneId bone) const noexcept
    {
        const auto* joints = boneArray();
        if (!joints)
            return {};

        const auto index = static_cast<std::size_t>(bone);
        if (index >= cs2::kBoneCount)
            return {};

        return joints[index].pos;
    }

    [[nodiscard]] const cs2::BoneJointData* boneArray() const noexcept
    {
        if (!playerPawn)
            return nullptr;

        auto* sceneNode = hookContext.patternSearchResults().template get<OffsetToGameSceneNode>().of(playerPawn).valueOr(nullptr);
        if (!sceneNode)
            return nullptr;

        const auto* boneArrayPtr = *reinterpret_cast<cs2::BoneJointData* const*>(
            reinterpret_cast<const std::byte*>(sceneNode) + cs2::combat_offsets::kBoneArray);
        return boneArrayPtr;
    }

private:
    HookContext& hookContext;
    cs2::C_CSPlayerPawn* playerPawn;
};
