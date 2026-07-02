#pragma once

#include <cstddef>

#include <CS2/Constants/DllNames.h>
#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()
#include <Platform/Windows/PebLdr.h>
#include <Platform/Windows/WindowsPlatformApi.h>
#elif IS_LINUX()
#include <Platform/Linux/LinuxDynamicLibrary.h>
#endif

[[nodiscard]] inline std::byte* getClientDllBase() noexcept
{
#if IS_WIN64()
    return reinterpret_cast<std::byte*>(PebLdr{WindowsPlatformApi::getPeb()->ldr}.getModuleHandle(cs2::CLIENT_DLL));
#elif IS_LINUX()
    const LinuxDynamicLibrary clientDll{cs2::CLIENT_DLL};
    if (const auto* linkMap = clientDll.getLinkMap())
        return reinterpret_cast<std::byte*>(linkMap->l_addr);
    return nullptr;
#else
    return nullptr;
#endif
}
