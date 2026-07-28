#pragma once

#include <MemoryPatterns/PatternTypes/InventoryPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct InventoryPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PointerToInventoryManagerGet, CodePattern{"E8 ? ? ? ? 48 8B D8 E8 ? ? ? ? 8B 70"}.add(1).abs()>()
            .template addPattern<PointerToCreateEconItem, CodePattern{"48 83 EC ? B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 48 8D 0D ? ? ? ? C7 40"}>()
            .template addPattern<PointerToSetDynamicAttributeValueUint, CodePattern{"E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 49 8B C0 48 8B CA 48 8B D0"}.add(1).abs()>()
            .template addPattern<PointerToGetEconItemSystem, CodePattern{"48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 0F 85 81"}>()
            .template addPattern<PointerToGetAttributeDefinitionInterface, CodePattern{"E8 ? ? ? ? 48 85 C0 74 ? E8 ? ? ? ? 0F B7 14 3B 48 8B C8 E8 ? ? ? ? 0F B6 48"}.add(1).abs()>()
            .template addPattern<PointerToGetItemInLoadout, CodePattern{"40 55 48 83 EC ? 49 63 E8"}>()
            .template addPattern<PointerToEquipItemInLoadout, CodePattern{"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 89 54 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F B7 FA"}>()
            .template addPattern<PointerToCreateBaseTypeCache, CodePattern{"E8 ? ? ? ? 41 8B D5 49 8B CD"}.add(1).abs()>()
            .template addPattern<PointerToFindTypeCache, CodePattern{"4C 8B 49 18 44 8B D2 4C 63 41 10 4F 8D 1C C1 49 8B C3"}>()
            .template addPattern<PointerToUpdateSkin, CodePattern{"48 89 5C 24 08 57 48 83 EC 20 8B DA 48 8B F9 E8"}>()
            .template addPattern<PointerToSetAttributeValueByName, CodePattern{"E8 ? ? ? ? 66 41 0F 6E D4"}.add(1).abs()>()
            .template addPattern<PointerToGetStaticData, CodePattern{"40 56 48 83 EC ? 48 89 5C 24 ? 48 8B F1 48 8B 1D"}>()
            .template addPattern<PointerToSetModel, CodePattern{"40 53 48 83 EC ? 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24 40"}>()
            .template addPattern<PointerToSetMeshGroupMask, CodePattern{"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8D 99 ? ? ? ? 48 8B 71"}>();
    }
};
