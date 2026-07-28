#pragma once

#include <algorithm>
#include <cstdint>

#include <CS2/Econ/CEconItem.h>
#include <CS2/Econ/CEconItemDefinition.h>
#include <CS2/Econ/InventoryOffsets.h>
#include <GameClient/Inventory/InventoryManager.h>
#include <HookContext/HookContextMacros.h>
#include <MemoryPatterns/PatternTypes/InventoryPatternTypes.h>

#include "InventoryChangerState.h"

template <typename HookContext>
class InventoryItemsManager {
public:
    struct AddResult {
        std::uint64_t itemId{};
        InventoryChangerState::AddedMeta meta{};
        InventoryApplyStatus failure{InventoryApplyStatus::None};
    };

    explicit InventoryItemsManager(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool patternsReady() const noexcept
    {
        const auto& patterns = hookContext.patternSearchResults();
        return patterns.template get<PointerToInventoryManagerGet>()
            && patterns.template get<PointerToCreateEconItem>()
            && patterns.template get<PointerToGetEconItemSystem>()
            && patterns.template get<PointerToSetDynamicAttributeValueUint>()
            && patterns.template get<PointerToGetAttributeDefinitionInterface>()
            && patterns.template get<PointerToFindTypeCache>()
            && patterns.template get<PointerToCreateBaseTypeCache>()
            && patterns.template get<PointerToEquipItemInLoadout>();
    }

    [[nodiscard]] AddResult addSkin(std::uint16_t defIndex, int paintKit, float wear, int seed, int statTrak, bool equip, int team, int slot) noexcept
    {
        AddResult result{};
        if (!patternsReady()) {
            result.failure = InventoryApplyStatus::PatternsNotReady;
            return result;
        }
        if (defIndex == 0) {
            result.failure = InventoryApplyStatus::ItemDefNotFound;
            return result;
        }

        InventoryManager<HookContext> inventoryManager{hookContext};
        auto inventory = inventoryManager.localInventory();
        if (!inventory) {
            result.failure = InventoryApplyStatus::InventoryNotReady;
            return result;
        }

        auto schema = inventoryManager.schema();
        if (!schema) {
            result.failure = InventoryApplyStatus::SchemaNotReady;
            return result;
        }

        auto& defs = schema.itemDefinitions();
        if (!defs.data || defs.size <= 0 || defs.size > 20000) {
            result.failure = InventoryApplyStatus::SchemaNotReady;
            return result;
        }

        cs2::CEconItemDefinition* foundDef = defs.findValue([defIndex](const auto& node) noexcept {
            return node.value && node.value->defIndex() == defIndex;
        });
        if (!foundDef) {
            result.failure = InventoryApplyStatus::ItemDefNotFound;
            return result;
        }

        cs2::CPaintKit* foundPaint = nullptr;
        if (paintKit > 0) {
            auto& paints = schema.paintKits();
            if (paints.data && paints.size > 0 && paints.size < 20000) {
                foundPaint = paints.findValue([paintKit](const auto& node) noexcept {
                    return node.value && node.value->nID == paintKit;
                });
            }
        }

        auto* item = inventoryManager.createItem();
        if (!item) {
            result.failure = InventoryApplyStatus::CreateItemFailed;
            return result;
        }

        const auto ids = inventory.highestIds();
        item->m_ulID = ids.first + 1;
        item->m_unInventory = ids.second + 1;
        item->m_unAccountID = static_cast<std::uint32_t>(inventory.owner().id);
        item->m_unDefIndex = defIndex;

        const bool unusual = foundDef->isKnife(true) || foundDef->isGlove(true) || foundDef->isAgent(true);
        if (unusual)
            item->m_nQuality = cs2::IQ_UNUSUAL;

        const int skinRarity = foundPaint ? foundPaint->nRarity : 0;
        item->m_nRarity = static_cast<std::uint16_t>((std::max)(0, (std::min)(foundDef->itemRarity() + skinRarity - 1, skinRarity == 7 ? 7 : 6)));

        if (paintKit > 0)
            inventoryManager.setPaintKit(item, static_cast<float>(paintKit));
        if (wear > 0.0f)
            inventoryManager.setPaintWear(item, wear);
        if (seed > 0)
            inventoryManager.setPaintSeed(item, static_cast<float>(seed));
        if (statTrak >= 0)
            inventoryManager.setStatTrak(item, statTrak);

        if (!inventory.addEconItem(item)) {
            result.failure = InventoryApplyStatus::AddEconItemFailed;
            return result;
        }

        result.itemId = item->m_ulID;
        result.meta.itemId = item->m_ulID;
        result.meta.defIndex = defIndex;
        result.meta.paintKit = paintKit;
        result.meta.legacyModel = foundPaint ? foundPaint->usesLegacyModel() : false;
        result.meta.isKnife = foundDef->isKnife(true);

        if (equip) {
            const int useSlot = slot >= 0 ? slot : static_cast<int>(foundDef->loadoutSlot());
            (void)inventoryManager.equipItemInLoadout(2, useSlot, item->m_ulID);
            (void)inventoryManager.equipItemInLoadout(3, useSlot, item->m_ulID);
        }

        return result;
    }

private:
    HookContext& hookContext;
};
