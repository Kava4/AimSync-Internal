#pragma once

#include <cstdint>

#include <Utils/DynamicArray.h>

enum class InventoryApplyStatus : std::uint8_t {
    None = 0,
    Success,
    PatternsNotReady,
    InventoryNotReady,
    SchemaNotReady,
    ItemDefNotFound,
    CreateItemFailed,
    AddEconItemFailed,
    NoSkinsSelected,
};

struct InventoryChangerState {
    struct AddedMeta {
        std::uint64_t itemId{};
        bool legacyModel{false};
        bool isKnife{false};
        int paintKit{0};
        std::uint16_t defIndex{0};
    };

    bool seeded{false};
    bool applyInProgress{false};
    int lastItemsAdded{0};
    int skinDirtyCounter{0};
    bool lastApplyPatternsReady{true};
    bool lastApplyInventoryReady{true};
    InventoryApplyStatus lastApplyStatus{InventoryApplyStatus::None};
    std::uint16_t lastFailedDefIndex{};
    DynamicArray<AddedMeta> addedItems;
    DynamicArray<void*> skinsRefreshedWeapons;
};
