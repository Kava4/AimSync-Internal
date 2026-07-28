#pragma once

#include <GameClient/Entities/PlayerPawn.h>

#include "NoSpreadConfigVariables.h"

template <typename HookContext>
class NoSpread {
public:
    explicit NoSpread(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() noexcept
    {
        if (!GET_CONFIG_VAR(no_spread_vars::Enabled))
            return;

        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn || !localPlayerPawn.isAlive().value_or(false))
            return;

        // Zero client-side accuracy buildup. Base weapon spread still exists, but
        // standing/moving/firing inaccuracy (the main spray bloom) is removed.
        localPlayerPawn.getActiveWeapon().clearAccuracyPenalty();
    }

private:
    HookContext& hookContext;
};
