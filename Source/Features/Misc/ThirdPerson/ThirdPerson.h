#pragma once

#include <CS2/Classes/ConVarTypes.h>

#include "ThirdPersonConfigVariables.h"

template <typename HookContext>
class ThirdPerson {
public:
    explicit ThirdPerson(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() const noexcept
    {
        const bool enabled = GET_CONFIG_VAR(thirdperson_vars::Enabled);
        hookContext.cvarSystem().template setConVarValue<cs2::c_thirdpersonshoulder>(enabled);
        if (enabled)
            hookContext.cvarSystem().template setConVarValue<cs2::cam_idealdist>(static_cast<float>(GET_CONFIG_VAR(thirdperson_vars::Distance)));
    }

    void onDisable() const noexcept
    {
        hookContext.cvarSystem().template setConVarValue<cs2::c_thirdpersonshoulder>(false);
    }

private:
    HookContext& hookContext;
};
