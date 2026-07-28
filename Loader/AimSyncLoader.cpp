#include <Windows.h>
#include <TlHelp32.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include "injector.h"

namespace
{

constexpr wchar_t kProcessName[] = L"cs2.exe";
constexpr int kResourceId = 101;
constexpr int kWaitTimeoutMs = 120000;
constexpr int kPollIntervalMs = 500;

void enableDebugPrivilege() noexcept
{
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
        return;

    TOKEN_PRIVILEGES privileges{};
    privileges.PrivilegeCount = 1;
    privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &privileges.Privileges[0].Luid))
        AdjustTokenPrivileges(token, FALSE, &privileges, 0, nullptr, nullptr);

    CloseHandle(token);
}

[[nodiscard]] DWORD findProcessId(const wchar_t* processName) noexcept
{
    PROCESSENTRY32W entry{.dwSize = sizeof(entry)};
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    DWORD processId = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName) == 0) {
                processId = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return processId;
}

[[nodiscard]] bool isSameArchitecture(HANDLE process) noexcept
{
    BOOL targetWow64 = FALSE;
    if (!IsWow64Process(process, &targetWow64))
        return false;

    BOOL hostWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &hostWow64);
    return targetWow64 == hostWow64;
}

[[nodiscard]] bool loadEmbeddedDll(std::vector<BYTE>& out)
{
    const HRSRC resource = FindResourceW(nullptr, MAKEINTRESOURCEW(kResourceId), RT_RCDATA);
    if (!resource)
        return false;

    const HGLOBAL loaded = LoadResource(nullptr, resource);
    if (!loaded)
        return false;

    const auto* data = static_cast<const BYTE*>(LockResource(loaded));
    const DWORD size = SizeofResource(nullptr, resource);
    if (!data || size < 0x1000)
        return false;

    out.assign(data, data + size);
    return true;
}

} // namespace

int wmain()
{
    SetConsoleTitleW(L"AimSync Loader");
    enableDebugPrivilege();

    std::wcout << L"[AimSync] Waiting for cs2.exe...\n";

    DWORD processId = 0;
    for (int elapsed = 0; elapsed < kWaitTimeoutMs; elapsed += kPollIntervalMs) {
        processId = findProcessId(kProcessName);
        if (processId != 0)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(kPollIntervalMs));
    }

    if (processId == 0) {
        std::wcerr << L"[AimSync] Timed out waiting for cs2.exe.\n";
        system("pause");
        return 1;
    }

    std::wcout << L"[AimSync] Found cs2.exe (PID " << processId << L"). Loading payload...\n";

    std::vector<BYTE> dllBytes;
    if (!loadEmbeddedDll(dllBytes)) {
        std::wcerr << L"[AimSync] Failed to load embedded DLL resource.\n";
        system("pause");
        return 1;
    }

    const HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!process) {
        std::wcerr << L"[AimSync] OpenProcess failed (error " << GetLastError()
                   << L"). Run as Administrator.\n";
        system("pause");
        return 1;
    }

    if (!isSameArchitecture(process)) {
        std::wcerr << L"[AimSync] Architecture mismatch (need x64 cs2 + x64 loader).\n";
        CloseHandle(process);
        system("pause");
        return 1;
    }

    std::wcout << L"[AimSync] Manual mapping (" << dllBytes.size() << L" bytes)...\n";
    const bool ok = ManualMapDll(process, dllBytes.data(), dllBytes.size());
    CloseHandle(process);

    if (!ok) {
        std::wcerr << L"[AimSync] Manual map failed.\n";
        system("pause");
        return 1;
    }

    std::wcout << L"[AimSync] Injected successfully.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
