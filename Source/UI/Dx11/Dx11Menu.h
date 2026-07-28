#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()

struct IDXGISwapChain;

void dx11MenuOnPresent(IDXGISwapChain* swapChain) noexcept;
void dx11MenuOnResizeBuffers() noexcept;
void dx11MenuOnCreateSwapChain() noexcept;
void dx11MenuOnUnload() noexcept;
void dx11MenuTryDeferredInstall() noexcept;
void dx11MenuRequestUnload() noexcept;
[[nodiscard]] bool dx11MenuConsumeUnloadRequest() noexcept;

void aimSyncOnFrameStageNotify(int frameStage) noexcept;
void aimSyncOnSOCacheSubscribed() noexcept;
void aimSyncOnEquipItemInLoadout(int team, int slot, unsigned long long itemId) noexcept;

[[nodiscard]] bool isDx11MenuActive() noexcept;
[[nodiscard]] bool isDx11MenuVisible() noexcept;
[[nodiscard]] bool shouldUsePanoramaMenu() noexcept;

#else

inline void dx11MenuOnUnload() noexcept {}
inline void dx11MenuTryDeferredInstall() noexcept {}
inline void dx11MenuRequestUnload() noexcept {}
[[nodiscard]] inline bool dx11MenuConsumeUnloadRequest() noexcept { return false; }
[[nodiscard]] inline bool isDx11MenuActive() noexcept { return false; }
[[nodiscard]] inline bool isDx11MenuVisible() noexcept { return false; }
[[nodiscard]] inline bool shouldUsePanoramaMenu() noexcept { return true; }

#endif
