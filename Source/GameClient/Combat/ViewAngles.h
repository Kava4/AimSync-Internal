#pragma once

#include <CS2/Classes/QAngle.h>
#include <CS2/Combat/CombatOffsets.h>
#include <Utils/Optional.h>

#include "ClientDllBase.h"

struct ViewAngles {
    [[nodiscard]] Optional<cs2::QAngle> get() const noexcept
    {
        if (auto* angles = getAngles())
            return *angles;
        return {};
    }

    void set(const cs2::QAngle& angles) const noexcept
    {
        if (auto* viewAngles = getAngles())
            *viewAngles = angles;
    }

private:
    // dwViewAngles is a standalone client.dll global (QAngle*), not a field inside CCSGOInput.
    // Writing into CCSGOInput+delta corrupts input/button state and breaks spray.
    [[nodiscard]] cs2::QAngle* getAngles() const noexcept
    {
        const auto clientBase = getClientDllBase();
        if (!clientBase)
            return nullptr;
        return reinterpret_cast<cs2::QAngle*>(clientBase + cs2::combat_offsets::kViewAngles);
    }
};
