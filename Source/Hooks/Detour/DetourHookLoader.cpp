#include "DetourHookLoader.h"

#include <CS2/Constants/DllNames.h>
#include <MemorySearch/RuntimePatternSearch.h>
#include <Platform/DynamicLibrary.h>
#include <ThirdParty/MinHook/MinHook.h>
#include <UI/Dx11/Dx11Menu.h>

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

using Present_t = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
using ResizeBuffers_t = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
using CreateSwapChain_t = HRESULT(WINAPI*)(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
using FrameStageNotify_t = void(__fastcall*)(void*, int);
using SOCacheSubscribed_t = void*(__fastcall*)(void*, void*, std::int64_t);

static Present_t dxgiPresentOriginal = nullptr;
static Present_t steamPresentOriginal = nullptr;
static ResizeBuffers_t dxgiResizeBuffersOriginal = nullptr;
static ResizeBuffers_t steamResizeBuffersOriginal = nullptr;
static CreateSwapChain_t createSwapChainOriginal = nullptr;
static FrameStageNotify_t frameStageNotifyOriginal = nullptr;
static SOCacheSubscribed_t soCacheSubscribedOriginal = nullptr;
static bool minHookInitialized = false;
static bool overlayHooksInstalled = false;
static bool clientHooksInstalled = false;
static bool dxgiPresentHookInstalled = false;
static bool steamPresentHookInstalled = false;

static HRESULT __stdcall hookDxgiPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) noexcept
{
    dx11MenuOnPresent(swapChain);
    return dxgiPresentOriginal(swapChain, syncInterval, flags);
}

static HRESULT __stdcall hookSteamPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) noexcept
{
    dx11MenuOnPresent(swapChain);
    return steamPresentOriginal(swapChain, syncInterval, flags);
}

static HRESULT __stdcall hookDxgiResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) noexcept
{
    dx11MenuOnResizeBuffers();
    return dxgiResizeBuffersOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

static HRESULT __stdcall hookSteamResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) noexcept
{
    dx11MenuOnResizeBuffers();
    return steamResizeBuffersOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

static HRESULT WINAPI hookCreateSwapChain(IDXGIFactory* factory, IUnknown* device, DXGI_SWAP_CHAIN_DESC* desc, IDXGISwapChain** swapChain) noexcept
{
    dx11MenuOnCreateSwapChain();
    return createSwapChainOriginal(factory, device, desc, swapChain);
}

static void __fastcall hookFrameStageNotify(void* client, int frameStage) noexcept
{
    // Run original first so net updates settle, then patch weapon skins for this frame.
    frameStageNotifyOriginal(client, frameStage);
    aimSyncOnFrameStageNotify(frameStage);
}

static void* __fastcall hookSOCacheSubscribed(void* inventory, void* owner, std::int64_t unk) noexcept
{
    const auto result = soCacheSubscribedOriginal(inventory, owner, unk);
    aimSyncOnSOCacheSubscribed();
    return result;
}

namespace
{

bool moduleLoaded(const char* moduleName) noexcept
{
    return static_cast<bool>(DynamicLibrary{moduleName});
}

bool createHook(void* target, void* detour, void** original) noexcept
{
    if (!target || !detour || !original)
        return false;
    if (*original != nullptr)
        return true;
    return MH_CreateHook(target, detour, original) == MH_OK;
}

bool installPatternHook(const char* moduleName, const char* pattern, void* detour, void** original) noexcept
{
    if (!moduleLoaded(moduleName))
        return false;

    DynamicLibrary library{moduleName};
    void* const target = runtime_pattern_search::findInModuleImage(library, pattern);
    if (!target)
        return false;

    return createHook(target, detour, original);
}

bool installDxgiSwapChainVtableHooks() noexcept
{
    if (dxgiPresentHookInstalled)
        return true;

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.lpfnWndProc = DefWindowProcW;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.lpszClassName = L"AimSyncDx11Hook";

    if (!RegisterClassExW(&windowClass))
        return false;

    HWND window = CreateWindowExW(
        0,
        windowClass.lpszClassName,
        L"",
        WS_OVERLAPPEDWINDOW,
        0, 0, 100, 100,
        nullptr,
        nullptr,
        windowClass.hInstance,
        nullptr);

    if (!window) {
        UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = window;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    const HRESULT createResult = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &swapChain,
        &device,
        nullptr,
        &context);

    bool hooked = false;
    if (SUCCEEDED(createResult) && swapChain) {
        auto* const vtable = *reinterpret_cast<void***>(swapChain);
        hooked = createHook(vtable[8], reinterpret_cast<void*>(&hookDxgiPresent), reinterpret_cast<void**>(&dxgiPresentOriginal));
        createHook(vtable[13], reinterpret_cast<void*>(&hookDxgiResizeBuffers), reinterpret_cast<void**>(&dxgiResizeBuffersOriginal));
    }

    if (swapChain)
        swapChain->Release();
    if (context)
        context->Release();
    if (device)
        device->Release();

    DestroyWindow(window);
    UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);

    if (hooked)
        dxgiPresentHookInstalled = true;

    return hooked;
}

bool installSteamOverlayHooks() noexcept
{
    if (steamPresentHookInstalled)
        return true;

    if (!moduleLoaded(cs2::GAMEOVERLAYRENDERER_DLL))
        return false;

    constexpr auto kPresent = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B E8";
    constexpr auto kResizeBuffers = "40 53 55 56 57 41 54 41 56 41 57 48 83 EC ? 44 8B E2";
    constexpr auto kCreateSwapChain = "40 53 55 56 57 48 83 EC ? 48 8B F9 49 8B F1 48 8D 0D ? ? ? ? 49 8B D8 48 8B EA E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 4C 8B CE 4C 8B C3 48 8B D5 48 8B CF FF D0 8B D8 85 C0 78 ? 48 85 F6 74 ? 48 83 3E ? 74 ? 48 8B D5 48 8B CE E8 ? ? ? ? 8B C3 48 83 C4 ? 5F 5E 5D 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 83 EC";

    bool hooked = false;
    if (installPatternHook(cs2::GAMEOVERLAYRENDERER_DLL, kPresent, reinterpret_cast<void*>(&hookSteamPresent), reinterpret_cast<void**>(&steamPresentOriginal)))
        hooked = true;
    installPatternHook(cs2::GAMEOVERLAYRENDERER_DLL, kResizeBuffers, reinterpret_cast<void*>(&hookSteamResizeBuffers), reinterpret_cast<void**>(&steamResizeBuffersOriginal));
    installPatternHook(cs2::GAMEOVERLAYRENDERER_DLL, kCreateSwapChain, reinterpret_cast<void*>(&hookCreateSwapChain), reinterpret_cast<void**>(&createSwapChainOriginal));

    if (hooked)
        steamPresentHookInstalled = true;

    return hooked;
}

}

bool DetourHookLoader::initializeMinHook() noexcept
{
    if (minHookInitialized)
        return true;
    if (MH_Initialize() != MH_OK)
        return false;
    minHookInitialized = true;
    return true;
}

bool DetourHookLoader::installOverlayHooks(bool /*waitForModules*/) noexcept
{
    if (overlayHooksInstalled)
        return true;

    if (!initializeMinHook())
        return false;

    bool hooked = false;

    // Primary path: hook IDXGISwapChain vtable Present in dxgi.dll (works without Steam overlay sigs).
    if (installDxgiSwapChainVtableHooks())
        hooked = true;

    // Secondary path: Steam overlay renderer (when enabled and sigs match).
    if (installSteamOverlayHooks())
        hooked = true;

    if (!hooked)
        return false;

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
        return false;

    overlayHooksInstalled = true;
    return true;
}

void DetourHookLoader::installClientHooksOptional() noexcept
{
    // Inventory changer is paused — skip client.dll hooks (bad FSN/SOCache
    // patterns can freeze the game after inject).
    if (clientHooksInstalled)
        return;
    clientHooksInstalled = true;
}

void DetourHookLoader::uninstallAll() noexcept
{
    if (!minHookInitialized)
        return;

    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    minHookInitialized = false;
    overlayHooksInstalled = false;
    clientHooksInstalled = false;
    dxgiPresentHookInstalled = false;
    steamPresentHookInstalled = false;
    dxgiPresentOriginal = nullptr;
    steamPresentOriginal = nullptr;
    dxgiResizeBuffersOriginal = nullptr;
    steamResizeBuffersOriginal = nullptr;
}
