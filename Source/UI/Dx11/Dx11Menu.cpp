#include "Dx11Menu.h"

#include "AimSyncGui.h"

#include <Features/Combat/Aimbot/Aimbot.h>
#include <Features/Visuals/PlayerEsp/PlayerEsp.h>
#include <GlobalContext/GlobalContext.h>
#include <HookContext/HookContext.h>
#include <Hooks/Detour/DetourHookLoader.h>

#include <ThirdParty/ImGui/imgui.h>
#include <ThirdParty/ImGui/imgui_impl_dx11.h>
#include <ThirdParty/ImGui/imgui_impl_win32.h>

#include <Windows.h>
#include <cstdint>
#include <d3d11.h>
#include <dxgi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{

struct Dx11MenuState {
    bool initialized = false;
    bool visible = false;
    HWND window = nullptr;
    WNDPROC originalWndProc = nullptr;
    ImGuiContext* context = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11RenderTargetView* savedRenderTarget = nullptr;
};

Dx11MenuState g_menu{};

enum class Dx11InstallState : std::uint8_t {
    Pending = 0,
    Active = 1,
};

Dx11InstallState g_installState = Dx11InstallState::Pending;
bool g_menuToggleWasDown = false;
bool g_unloadRequested = false;
volatile LONG g_presentBusy = 0;

constexpr int kMenuToggleKeys[] = {VK_INSERT, VK_PAUSE};

void toggleMenu() noexcept;
void ensureImGuiContext() noexcept;
void renderFrameUnlocked(IDXGISwapChain* swapChain) noexcept;

void ensureImGuiContext() noexcept
{
    if (g_menu.context)
        ImGui::SetCurrentContext(g_menu.context);
}

bool isMenuToggleKeyDown() noexcept
{
    for (const int key : kMenuToggleKeys) {
        if ((GetAsyncKeyState(key) & 0x8000) != 0)
            return true;
    }
    return false;
}

void pollMenuToggleKey() noexcept
{
    const bool toggleDown = isMenuToggleKeyDown();
    if (toggleDown && !g_menuToggleWasDown)
        toggleMenu();
    g_menuToggleWasDown = toggleDown;
}

void createRenderTarget(IDXGISwapChain* swapChain) noexcept
{
    if (g_menu.renderTargetView || !g_menu.device)
        return;

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))) || !backBuffer)
        return;

    if (FAILED(g_menu.device->CreateRenderTargetView(backBuffer, nullptr, &g_menu.renderTargetView)))
        g_menu.renderTargetView = nullptr;

    backBuffer->Release();
}

void* imguiAllocate(std::size_t size, void*) noexcept
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void imguiFree(void* ptr, void*) noexcept
{
    if (ptr)
        HeapFree(GetProcessHeap(), 0, ptr);
}

void clearRenderTarget() noexcept
{
    if (g_menu.renderTargetView) {
        g_menu.renderTargetView->Release();
        g_menu.renderTargetView = nullptr;
    }
}

void toggleMenu() noexcept
{
    g_menu.visible = !g_menu.visible;
    if (g_menu.initialized) {
        ensureImGuiContext();
        ImGui::GetIO().MouseDrawCursor = g_menu.visible;
    }
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (g_menu.initialized && g_menu.visible) {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam) != 0)
            return TRUE;
    }
    return CallWindowProcW(g_menu.originalWndProc, hwnd, msg, wParam, lParam);
}

void initializeMenu(IDXGISwapChain* swapChain) noexcept
{
    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&g_menu.device))))
        return;

    g_menu.device->GetImmediateContext(&g_menu.deviceContext);
    if (FAILED(swapChain->GetDesc(&desc)) || !desc.OutputWindow)
        return;

    g_menu.window = desc.OutputWindow;
    ImGui::SetAllocatorFunctions(imguiAllocate, imguiFree, nullptr);
    g_menu.context = ImGui::CreateContext();
    ImGui::SetCurrentContext(g_menu.context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    // Keep default ImGui font — no AddFontFromFile (was crashing on Present thread).

    ImGui_ImplWin32_Init(g_menu.window);
    ImGui_ImplDX11_Init(g_menu.device, g_menu.deviceContext);
    aimsync_gui::applyStyle();

    g_menu.originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(g_menu.window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&wndProc)));
    g_menu.initialized = true;
    g_menu.visible = false;
    ImGui::GetIO().MouseDrawCursor = false;
}

void renderFrameUnlocked(IDXGISwapChain* swapChain) noexcept
{
    ensureImGuiContext();
    pollMenuToggleKey();
    createRenderTarget(swapChain);

    if (!g_menu.renderTargetView)
        return;

    g_menu.deviceContext->OMGetRenderTargets(1, &g_menu.savedRenderTarget, nullptr);
    g_menu.deviceContext->OMSetRenderTargets(1, &g_menu.renderTargetView, nullptr);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (HookContext<GlobalContext>::isGlobalContextComplete()) {
        HookContext<GlobalContext> hookContext;
        hookContext.template make<PlayerEsp>().draw();
        hookContext.template make<Aimbot>().drawFov();

        if (g_menu.visible)
            aimsync_gui::draw(hookContext);
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_menu.deviceContext->OMSetRenderTargets(1, &g_menu.savedRenderTarget, nullptr);
    if (g_menu.savedRenderTarget) {
        g_menu.savedRenderTarget->Release();
        g_menu.savedRenderTarget = nullptr;
    }
}

void shutdownMenu() noexcept
{
    if (!g_menu.initialized)
        return;

    if (g_menu.window && g_menu.originalWndProc)
        SetWindowLongPtrW(g_menu.window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_menu.originalWndProc));

    clearRenderTarget();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(g_menu.context);

    if (g_menu.deviceContext)
        g_menu.deviceContext->Release();
    if (g_menu.device)
        g_menu.device->Release();

    g_menu = {};
}

}

bool isDx11MenuActive() noexcept
{
    return g_installState == Dx11InstallState::Active;
}

bool isDx11MenuVisible() noexcept
{
    return g_menu.initialized && g_menu.visible;
}

bool shouldUsePanoramaMenu() noexcept
{
    return false;
}

void dx11MenuTryDeferredInstall() noexcept
{
    if (g_installState == Dx11InstallState::Active)
        return;

    if (DetourHookLoader::installOverlayHooks(false)) {
        DetourHookLoader::installClientHooksOptional();
        g_installState = Dx11InstallState::Active;
    }
}

void dx11MenuOnPresent(IDXGISwapChain* swapChain) noexcept
{
    if (!swapChain)
        return;

    if (InterlockedCompareExchange(&g_presentBusy, 1, 0) != 0)
        return;

    if (!g_menu.initialized)
        initializeMenu(swapChain);
    else
        renderFrameUnlocked(swapChain);

    InterlockedExchange(&g_presentBusy, 0);
}

void dx11MenuOnResizeBuffers() noexcept
{
    clearRenderTarget();
}

void dx11MenuOnCreateSwapChain() noexcept
{
    clearRenderTarget();
}

void dx11MenuOnUnload() noexcept
{
    shutdownMenu();
    g_installState = Dx11InstallState::Pending;
}

void dx11MenuRequestUnload() noexcept
{
    g_unloadRequested = true;
}

bool dx11MenuConsumeUnloadRequest() noexcept
{
    if (!g_unloadRequested)
        return false;
    g_unloadRequested = false;
    return true;
}

void aimSyncOnFrameStageNotify(int) noexcept {}
void aimSyncOnSOCacheSubscribed() noexcept {}
void aimSyncOnEquipItemInLoadout(int, int, unsigned long long) noexcept {}
