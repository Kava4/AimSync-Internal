#include <Windows.h>

namespace
{
bool g_updateSkinAllowed = true;
}

extern "C" void aimSyncTryUpdateSkin(void (*fn)(void* weapon, bool update), void* weapon) noexcept
{
    if (!g_updateSkinAllowed || !fn || !weapon)
        return;

    __try {
        fn(weapon, true);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        g_updateSkinAllowed = false;
    }
}
