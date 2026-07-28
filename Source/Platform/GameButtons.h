#pragma once

#ifdef _WIN64
#include <Platform/Windows/WindowsGameButtons.h>
using AttackInput = WindowsAttackInput;
using JumpInput = WindowsJumpInput;
using MoveButtons = WindowsMoveButtons;
#else
struct AttackInput {
    void press() const noexcept {}
    void release() const noexcept {}
    void clear() const noexcept {}
    [[nodiscard]] bool isDown() const noexcept { return false; }
};
struct JumpInput {
    void press() const noexcept {}
    void release() const noexcept {}
    void clear() const noexcept {}
    [[nodiscard]] bool isDown() const noexcept { return false; }
};
struct MoveButtons {
    void strafeLeft() const noexcept {}
    void strafeRight() const noexcept {}
    void clear() const noexcept {}
};
#endif
