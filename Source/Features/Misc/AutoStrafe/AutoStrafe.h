#pragma once

#include "AutoStrafeConfigVariables.h"

template <typename HookContext>
class AutoStrafe {
public:
    explicit AutoStrafe(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() const noexcept
    {
        // Never touch movement buttons unless the feature is actively strafing.
        // Clearing left/right every frame (even when disabled) blocks A/D input.
        if (!GET_CONFIG_VAR(autostrafe_vars::Enabled))
            return;

        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn || !localPlayerPawn.isAlive().value_or(false) || localPlayerPawn.isOnGround())
            return;

        // Placeholder: real autostrafe needs CreateMove/usercmd.
        // Do not write force A/D here — it fights keyboard input.
    }

private:
    HookContext& hookContext;
};
