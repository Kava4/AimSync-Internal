#pragma once

// Inventory changer is temporarily disabled (in-game apply caused crashes).
// UI tab shows paused; runtime hooks are no-ops.

#include "InventoryChangerConfigVariables.h"
#include "InventoryChangerState.h"

template <typename HookContext>
class InventoryChanger {
public:
    explicit InventoryChanger(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] int applySelectedSkinsToInventory() noexcept
    {
        (void)hookContext;
        return 0;
    }

    void update() noexcept
    {
    }

    void onFrameStageNotify(int frameStage) noexcept
    {
        (void)frameStage;
    }

    void onSOCacheSubscribed() noexcept
    {
    }

private:
    HookContext& hookContext;
};
