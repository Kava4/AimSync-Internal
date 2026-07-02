#pragma once

#include <cstddef>

#include <CS2/Constants/DllNames.h>
#include <Platform/Windows/PebLdr.h>
#include <Platform/Windows/WindowsPlatformApi.h>

#ifdef _WIN64
[[nodiscard]] inline std::byte* getClientDllBase() noexcept
{
    return reinterpret_cast<std::byte*>(PebLdr{WindowsPlatformApi::getPeb()->ldr}.getModuleHandle(cs2::CLIENT_DLL));
}
#else
[[nodiscard]] inline std::byte* getClientDllBase() noexcept
{
    return nullptr;
}
#endif
