#include <Windows.h>

namespace
{

HMODULE g_aimSyncModule{};

DWORD WINAPI unloadThread(LPVOID module) noexcept
{
    Sleep(200);
    FreeLibraryAndExitThread(static_cast<HMODULE>(module), 0);
}

}

void aimSyncSetModuleHandle(void* module) noexcept
{
    g_aimSyncModule = static_cast<HMODULE>(module);
}

void aimSyncFreeLibrary() noexcept
{
    if (!g_aimSyncModule)
        return;

    if (HANDLE thread = CreateThread(nullptr, 0, unloadThread, g_aimSyncModule, 0, nullptr))
        CloseHandle(thread);
}
