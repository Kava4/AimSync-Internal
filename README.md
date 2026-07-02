# AimSync Internal CS2

[![Windows](https://github.com/Kava-4/AimSync-Internal/actions/workflows/windows.yml/badge.svg)](https://github.com/Kava-4/AimSync-Internal/actions/workflows/windows.yml)
[![Linux](https://github.com/Kava-4/AimSync-Internal/actions/workflows/linux.yml/badge.svg)](https://github.com/Kava-4/AimSync-Internal/actions/workflows/linux.yml)

Open-source internal cheat for **Counter-Strike 2**. Native Panorama menu in the main lobby — no external overlay, no ImGui.

> **Internal · CS2** — pattern-scanned hooks, game-native UI, config auto-save.

---

## Features

### Aim
| Tab | Description |
|-----|-------------|
| **Snipers** | No-scope inaccuracy visualization |
| **Aimbot** | FOV, smoothness, team check, visibility check, RCS |
| **Trigger** | Crosshair trigger with delay and team check |
| **Skins** | *Work in progress* |

### Overlay
Bomb timer, defuse countdown, plant alert, killfeed preservation, post-round timer.

### ESP
| Tab | Description |
|-----|-------------|
| **Player ESP** | Health, weapon, ammo, position arrows, bomb/hostage/defuse icons |
| **Outlines** | Player, weapon, grenade, bomb, defuse kit, hostage glow |
| **Models** | Full model glow with live preview |
| **Viewmodel** | FOV modification with preview |

### Audio
Footstep, bomb plant/beep/defuse, scope and reload sound visualization.

---

## Menu

Open the main menu in CS2 lobby. Click the **sniper spotted** icon in the top navbar.

Settings save automatically to `default.cfg`.

---

## Build

### Windows

**Requirements:** Visual Studio 2022 — Desktop development with C++

```powershell
msbuild AimSync.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

Output: `x64\Release\AimSync.dll`

### Linux

**Requirements:** CMake 3.24+, g++ 11+ or clang++ 18+

```bash
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build -j $(nproc --all)
```

Output: `build/Source/libAimSync.so`

---

## Inject

### Windows

Use a **manual mapping** (reflective) injector. Standard `LoadLibrary` injection is blocked by CS2.

> Xenos and Extreme Injector are known to be flagged by VAC.

### Linux

```bash
sudo gdb -batch-silent -p $(pidof cs2) -ex "call (void*)dlopen(\"$PWD/libAimSync.so\", 2)"
```

> gdb is visible under `TracerPid` during injection — may be detected.

---

## Config

| Platform | Path |
|----------|------|
| Windows | `%appdata%\AimSyncCS2\configs\default.cfg` |
| Linux | `$HOME/AimSyncCS2/configs/default.cfg` |

---

## Technical

Release builds are designed for minimal footprint:

- No CRT in release builds
- No heap allocations
- No static imports (Windows release)
- No threads
- No exceptions
- No external dependencies

Offsets are sourced from [cs2-dumper](https://github.com/a2x/cs2-dumper) and must be updated after game patches.

---

## Disclaimer

For educational purposes only. Use at your own risk. The authors are not responsible for any bans or account restrictions.

---

## License

MIT — see [LICENSE](LICENSE).

Based on [Osiris](https://github.com/danielkrupinski/Osiris) by Daniel Krupiński.
