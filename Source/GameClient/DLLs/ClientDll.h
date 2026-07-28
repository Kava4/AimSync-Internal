#pragma once

#include <CS2/Constants/DllNames.h>
#include <Platform/DynamicLibrary.h>

struct ClientDll : DynamicLibrary {
    ClientDll() noexcept
        : DynamicLibrary{cs2::CLIENT_DLL}
    {
    }

    [[nodiscard]] void* source2Client() const noexcept
    {
        const auto createInterface = getFunctionAddress("CreateInterface").as<void*(*)(const char*, int*)>();
        if (!createInterface)
            return nullptr;
        return createInterface("Source2Client002", nullptr);
    }
};
