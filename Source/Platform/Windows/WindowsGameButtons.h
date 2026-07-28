#pragma once

#include <cstdint>

#include <CS2/Combat/CombatOffsets.h>
#include <GameClient/Combat/ClientDllBase.h>

struct WindowsGameButton {
    static constexpr std::int32_t kPressed = 65537;
    static constexpr std::int32_t kReleased = 256;

    explicit WindowsGameButton(std::ptrdiff_t offset) noexcept
        : offset{offset}
    {
    }

    void press() const noexcept
    {
        write(kPressed);
    }

    void release() const noexcept
    {
        write(kReleased);
    }

    void clear() const noexcept
    {
        write(0);
    }

    [[nodiscard]] bool isDown() const noexcept
    {
        if (auto* button = getButton())
            return (*button & 1) != 0;
        return false;
    }

private:
    void write(std::int32_t value) const noexcept
    {
        if (auto* button = getButton())
            *button = value;
    }

    [[nodiscard]] std::int32_t* getButton() const noexcept
    {
        const auto clientModule = getClientDllBase();
        if (!clientModule)
            return nullptr;
        return reinterpret_cast<std::int32_t*>(clientModule + offset);
    }

    std::ptrdiff_t offset;
};

struct WindowsAttackInput {
    void press() const noexcept { WindowsGameButton{cs2::combat_offsets::kAttackButton}.press(); }
    void release() const noexcept { WindowsGameButton{cs2::combat_offsets::kAttackButton}.release(); }
    void clear() const noexcept { WindowsGameButton{cs2::combat_offsets::kAttackButton}.clear(); }
    [[nodiscard]] bool isDown() const noexcept { return WindowsGameButton{cs2::combat_offsets::kAttackButton}.isDown(); }
};

struct WindowsJumpInput {
    void press() const noexcept { WindowsGameButton{cs2::combat_offsets::kJumpButton}.press(); }
    void release() const noexcept { WindowsGameButton{cs2::combat_offsets::kJumpButton}.release(); }
    void clear() const noexcept { WindowsGameButton{cs2::combat_offsets::kJumpButton}.clear(); }
    [[nodiscard]] bool isDown() const noexcept { return WindowsGameButton{cs2::combat_offsets::kJumpButton}.isDown(); }
};

struct WindowsMoveButtons {
    void strafeLeft() const noexcept
    {
        WindowsGameButton{cs2::combat_offsets::kRightButton}.release();
        WindowsGameButton{cs2::combat_offsets::kLeftButton}.press();
    }

    void strafeRight() const noexcept
    {
        WindowsGameButton{cs2::combat_offsets::kLeftButton}.release();
        WindowsGameButton{cs2::combat_offsets::kRightButton}.press();
    }

    void clear() const noexcept
    {
        WindowsGameButton{cs2::combat_offsets::kLeftButton}.clear();
        WindowsGameButton{cs2::combat_offsets::kRightButton}.clear();
    }
};
