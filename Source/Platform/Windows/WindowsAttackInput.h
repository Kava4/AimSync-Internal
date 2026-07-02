#pragma once

#include <cstdint>

#include <CS2/Combat/CombatOffsets.h>
#include <GameClient/Combat/ClientDllBase.h>

struct WindowsAttackInput {
    static constexpr std::int32_t kAttackPressed = 65537;
    static constexpr std::int32_t kAttackReleased = 256;

    void press() const noexcept
    {
        write(kAttackPressed);
    }

    void release() const noexcept
    {
        write(kAttackReleased);
    }

    void clear() const noexcept
    {
        write(0);
    }

private:
    void write(std::int32_t value) const noexcept
    {
        if (auto* attackButton = getAttackButton())
            *attackButton = value;
    }

    [[nodiscard]] std::int32_t* getAttackButton() const noexcept
    {
        const auto clientModule = getClientDllBase();
        if (!clientModule)
            return nullptr;
        return reinterpret_cast<std::int32_t*>(clientModule + cs2::combat_offsets::kAttackButton);
    }
};
