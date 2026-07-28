#pragma once

#include <CS2/Combat/CombatOffsets.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <Platform/GameButtons.h>

#include "BunnyHopConfigVariables.h"

template <typename HookContext>
class BunnyHop {
public:
    explicit BunnyHop(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() const noexcept
    {
        if (!GET_CONFIG_VAR(bunnyhop_vars::Enabled))
            return;

        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        if (!localPlayerPawn || !localPlayerPawn.isAlive().value_or(false))
            return;

        JumpInput jump{};
        if (!jump.isDown())
            return;

        if (localPlayerPawn.isOnGround())
            jump.press();
        else
            jump.release();
    }

private:
    HookContext& hookContext;
};
