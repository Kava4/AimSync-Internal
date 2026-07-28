#include "Dx11Menu.h"

// CMake Windows CI builds without the full DX11/ImGui/MinHook stack.
// MSBuild uses Dx11Menu.cpp + AimSyncUnload.cpp instead.

struct IDXGISwapChain;

void dx11MenuOnPresent(IDXGISwapChain*) noexcept {}
void dx11MenuOnResizeBuffers() noexcept {}
void dx11MenuOnCreateSwapChain() noexcept {}
void dx11MenuOnUnload() noexcept {}
void dx11MenuTryDeferredInstall() noexcept {}
void dx11MenuRequestUnload() noexcept {}
bool dx11MenuConsumeUnloadRequest() noexcept { return false; }

void aimSyncOnFrameStageNotify(int) noexcept {}
void aimSyncOnSOCacheSubscribed() noexcept {}
void aimSyncOnEquipItemInLoadout(int, int, unsigned long long) noexcept {}

bool isDx11MenuActive() noexcept { return false; }
bool isDx11MenuVisible() noexcept { return false; }
bool shouldUsePanoramaMenu() noexcept { return true; }

void aimSyncSetModuleHandle(void*) noexcept {}
void aimSyncFreeLibrary() noexcept {}
