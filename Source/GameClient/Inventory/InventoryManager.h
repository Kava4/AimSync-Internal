#pragma once

#include <algorithm>
#include <cstdint>
#include <utility>

#include <CS2/Econ/CEconItem.h>
#include <CS2/Econ/EconContainers.h>
#include <CS2/Econ/InventoryOffsets.h>
#include <GameClient/DLLs/ClientDll.h>
#include <MemoryPatterns/PatternTypes/InventoryPatternTypes.h>
#include <Utils/VirtualCall.h>

#include "EconItemSchema.h"

template <typename HookContext>
class PlayerInventory {
public:
    explicit PlayerInventory(HookContext& hookContext, void* inventory) noexcept
        : hookContext{hookContext}
        , inventory{inventory}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return inventory != nullptr;
    }

    [[nodiscard]] cs2::SOID_t owner() const noexcept
    {
        return *reinterpret_cast<cs2::SOID_t*>(reinterpret_cast<std::byte*>(inventory) + cs2::inventory_offsets::kPlayerInventoryOwner);
    }

    [[nodiscard]] void* itemInLoadout(int team, int slot) const noexcept
    {
        const auto fn = hookContext.patternSearchResults().template get<PointerToGetItemInLoadout>();
        return fn && inventory ? fn(inventory, team, slot) : nullptr;
    }

    [[nodiscard]] void* typeCache() const noexcept
    {
        const auto findFn = hookContext.patternSearchResults().template get<PointerToFindTypeCache>();
        const auto createFn = hookContext.patternSearchResults().template get<PointerToCreateBaseTypeCache>();
        if (!findFn || !createFn || !inventory)
            return nullptr;

        const std::ptrdiff_t soCacheOffsets[] = {
            cs2::inventory_offsets::kPlayerInventorySOCache,
            cs2::inventory_offsets::kPlayerInventorySOCacheAlt0,
            cs2::inventory_offsets::kPlayerInventorySOCacheAlt1,
        };

        for (const auto offset : soCacheOffsets) {
            auto* soCache = *reinterpret_cast<void**>(reinterpret_cast<std::byte*>(inventory) + offset);
            if (!soCache)
                continue;

            void* cache = findFn(soCache, cs2::k_EEconTypeItem);
            if (!cache)
                cache = createFn(soCache, cs2::k_EEconTypeItem);
            if (cache)
                return cache;
        }
        return nullptr;
    }

    [[nodiscard]] bool containsSocItem(std::uint64_t itemId) const noexcept
    {
        void* cache = typeCache();
        if (!cache)
            return false;

        const auto& objects = *reinterpret_cast<cs2::EconObjectVector<cs2::CEconItem*>*>(
            reinterpret_cast<std::byte*>(cache) + cs2::inventory_offsets::kSOTypeCacheObjects);

        for (int i = 0; i < objects.size; ++i) {
            if (objects.data[i] && objects.data[i]->m_ulID == itemId)
                return true;
        }
        return false;
    }

    void notifyItemAdded(const cs2::CEconItem* item) const noexcept
    {
        if (!item || !inventory)
            return;

        const auto owner = this->owner();
        const auto* sharedItem = reinterpret_cast<const void*>(item);
        // ISharedObjectListener: 0=SOCreated, 1=SOUpdated
        Utils::callVirtual<void>(inventory, 0, owner, sharedItem, cs2::eSOCacheEvent_Incremental);
        Utils::callVirtual<void>(inventory, 1, owner, sharedItem, cs2::eSOCacheEvent_Incremental);
    }

    [[nodiscard]] bool addEconItem(cs2::CEconItem* item) const noexcept
    {
        if (!item || !inventory)
            return false;

        void* cache = typeCache();
        if (!cache)
            return false;

        if (!Utils::callVirtual<bool>(cache, 1, reinterpret_cast<void*>(item)))
            return false;

        notifyItemAdded(item);
        return containsSocItem(item->m_ulID);
    }

    [[nodiscard]] std::pair<std::uint64_t, std::uint32_t> highestIds() const noexcept
    {
        std::uint64_t maxItemId = 0;
        std::uint32_t maxInventoryId = 0;

        void* cache = typeCache();
        if (!cache)
            return {maxItemId, maxInventoryId};

        const auto& objects = *reinterpret_cast<cs2::EconObjectVector<cs2::CEconItem*>*>(
            reinterpret_cast<std::byte*>(cache) + cs2::inventory_offsets::kSOTypeCacheObjects);

        for (int i = 0; i < objects.size; ++i) {
            auto* it = objects.data[i];
            if (!it || (it->m_ulID & 0xF000000000000000ULL) != 0)
                continue;
            maxItemId = (std::max)(maxItemId, it->m_ulID);
            maxInventoryId = (std::max)(maxInventoryId, it->m_unInventory);
        }
        return {maxItemId, maxInventoryId};
    }

private:
    HookContext& hookContext;
    void* inventory;
};

template <typename HookContext>
class InventoryManager {
public:
    explicit InventoryManager(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] void* raw() const noexcept
    {
        const auto getFn = hookContext.patternSearchResults().template get<PointerToInventoryManagerGet>();
        return getFn ? getFn() : nullptr;
    }

    [[nodiscard]] PlayerInventory<HookContext> localInventory() const noexcept
    {
        auto* manager = raw();
        if (!manager)
            return PlayerInventory<HookContext>{hookContext, nullptr};

        void* inventory = nullptr;
        const std::ptrdiff_t memberOffsets[] = {
            cs2::inventory_offsets::kInventoryManagerLocalInventoryAlt,
            cs2::inventory_offsets::kInventoryManagerLocalInventory,
        };
        for (const auto offset : memberOffsets) {
            auto* candidate = *reinterpret_cast<void**>(reinterpret_cast<std::byte*>(manager) + offset);
            if (candidate) {
                inventory = candidate;
                break;
            }
        }

        if (!inventory) {
            constexpr std::size_t vtableIndices[] = {59, 70};
            for (const auto index : vtableIndices) {
                inventory = Utils::callVirtual<void*>(manager, index);
                if (inventory)
                    break;
            }
        }

        return PlayerInventory<HookContext>{hookContext, inventory};
    }

    [[nodiscard]] bool equipItemInLoadout(int team, int slot, std::uint64_t itemId) const noexcept
    {
        const auto fn = hookContext.patternSearchResults().template get<PointerToEquipItemInLoadout>();
        auto* manager = raw();
        return fn && manager ? fn(manager, team, slot, itemId) : false;
    }

    [[nodiscard]] void* econItemSystem() const noexcept
    {
        const auto getSystem = hookContext.patternSearchResults().template get<PointerToGetEconItemSystem>();
        if (!getSystem)
            return nullptr;

        ClientDll clientDll;
        void* source2Client = clientDll.source2Client();
        if (!source2Client)
            return nullptr;

        if (void* system = getSystem(source2Client))
            return system;

        constexpr std::size_t vtableIndices[] = {116, 123, 59, 11};
        for (const auto index : vtableIndices) {
            if (void* system = Utils::callVirtual<void*>(source2Client, index))
                return system;
        }
        return nullptr;
    }

    [[nodiscard]] EconItemSchema<HookContext> schema() const noexcept
    {
        auto* system = econItemSystem();
        if (!system)
            return EconItemSchema<HookContext>{hookContext, nullptr};
        auto* schemaPtr = *reinterpret_cast<void**>(reinterpret_cast<std::byte*>(system) + cs2::inventory_offsets::kEconItemSystemSchema);
        return EconItemSchema<HookContext>{hookContext, schemaPtr};
    }

    [[nodiscard]] cs2::CEconItem* createItem() const noexcept
    {
        const auto fn = hookContext.patternSearchResults().template get<PointerToCreateEconItem>();
        return fn ? fn() : nullptr;
    }

    void setAttribute(cs2::CEconItem* item, int attributeIndex, void* value) const noexcept
    {
        if (!item || !value)
            return;
        auto schemaObj = schema();
        if (!schemaObj)
            return;
        auto* def = schemaObj.attributeDefinition(attributeIndex);
        const auto fn = hookContext.patternSearchResults().template get<PointerToSetDynamicAttributeValueUint>();
        if (def && fn)
            fn(item, def, value);
    }

    void setPaintKit(cs2::CEconItem* item, float paintKit) const noexcept
    {
        setAttribute(item, cs2::ATTRIBUTE_PAINT_KIT, &paintKit);
    }

    void setPaintWear(cs2::CEconItem* item, float wear) const noexcept
    {
        setAttribute(item, cs2::ATTRIBUTE_PAINT_WEAR, &wear);
    }

    void setPaintSeed(cs2::CEconItem* item, float seed) const noexcept
    {
        setAttribute(item, cs2::ATTRIBUTE_PAINT_SEED, &seed);
    }

    void setStatTrak(cs2::CEconItem* item, int count) const noexcept
    {
        setAttribute(item, cs2::ATTRIBUTE_STAT_TRACK, &count);
    }

private:
    HookContext& hookContext;
};
