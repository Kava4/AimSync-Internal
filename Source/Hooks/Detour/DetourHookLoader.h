#pragma once

#include <cstdint>

struct DetourHookLoader {
    [[nodiscard]] static bool initializeMinHook() noexcept;
    [[nodiscard]] static bool installOverlayHooks(bool waitForModules = false) noexcept;
    static void installClientHooksOptional() noexcept;
    static void uninstallAll() noexcept;
};
