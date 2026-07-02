#pragma once

#ifdef _WIN64
#include <Platform/Windows/WindowsAttackInput.h>
using AttackInput = WindowsAttackInput;
#else
struct AttackInput {
    void press() const noexcept {}
    void release() const noexcept {}
    void clear() const noexcept {}
};
#endif
