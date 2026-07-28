#pragma once

#include <cstddef>

#include <CS2/Econ/CEconItemDefinition.h>
#include <CS2/Econ/EconContainers.h>
#include <CS2/Econ/InventoryOffsets.h>
#include <MemoryPatterns/PatternTypes/InventoryPatternTypes.h>

template <typename HookContext>
class EconItemSchema {
public:
    EconItemSchema(HookContext& hookContext, void* schema) noexcept
        : hookContext{hookContext}
        , schema{schema}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return schema != nullptr;
    }

    [[nodiscard]] auto& itemDefinitions() const noexcept
    {
        return *reinterpret_cast<cs2::EconUtlMap<int, cs2::CEconItemDefinition*>*>(
            reinterpret_cast<std::byte*>(schema) + cs2::inventory_offsets::kSortedItemDefinitionMap);
    }

    [[nodiscard]] auto& paintKits() const noexcept
    {
        return *reinterpret_cast<cs2::EconUtlMap<int, cs2::CPaintKit*>*>(
            reinterpret_cast<std::byte*>(schema) + cs2::inventory_offsets::kPaintKits);
    }

    [[nodiscard]] void* attributeDefinition(int index) const noexcept
    {
        const auto fn = hookContext.patternSearchResults().template get<PointerToGetAttributeDefinitionInterface>();
        return fn && schema ? fn(schema, index) : nullptr;
    }

private:
    HookContext& hookContext;
    void* schema;
};
