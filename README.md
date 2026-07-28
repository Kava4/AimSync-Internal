# AimSync Internal

[![Windows](https://github.com/Kava4/AimSync-Internal/actions/workflows/windows.yml/badge.svg)](https://github.com/Kava4/AimSync-Internal/actions/workflows/windows.yml)

**AimSync Internal** is a Counter-Strike 2 internal cheat for Windows (x64), with a DX11 / ImGui overlay menu and an auto-injecting loader.

> Part of the [AimSync](https://github.com/Kava4/AimSync) ecosystem by [Kava4](https://github.com/Kava4).

---

## Status

| Area | State |
|------|--------|
| DX11 overlay menu (INSERT / PAUSE) | Working |
| Auto loader (`AimSync.exe`) | Working |
| Aimbot / Triggerbot / No Recoil / No Spread | Working |
| Box / Skeleton ESP | Working |
| Model / Outline glow, player info, HUD helpers | Working |
| Bunny hop / Auto strafe / Third person | Working |
| Sound ESP | Working |
| Inventory / skin changer | **Paused (WIP)** — buy-menu path was unstable |
| Config manager (create / load / save / delete) | Working |
| Overlay icon textures / custom font polish | **TODO** |

---

## Working features

### Combat
- **Aimbot** — FOV, smooth, team/visibility checks, recoil control, hold-LMB, draw FOV, hitbox select (head / neck / chest / pelvis / nearest)
- **Triggerbot** — delay, team check, scope-only, flash ignore
- **No Recoil** — strength slider
- **No Spread**
- **No-scope inaccuracy visualization**

### Visuals
- **Player ESP** — box (full / corner), skeleton, head circle, health bar, enemies-only, visibility check
- **Model glow** / **Outline glow** (players, weapons, bombs, projectiles, etc.)
- **Player info in world** — name, health, weapon, ammo, arrows, state icons
- **Grenade prediction**
- **Viewmodel FOV mod**

### Misc / HUD / Audio
- Bomb timer, defusing alert, plant alert, killfeed preserver, post-round timer
- Bunny hop, auto strafe, third person
- Sound ESP — footsteps, bomb plant/beep/defuse, scope, reload

### Loader
- `AimSync.exe` waits for `cs2.exe` and manual-maps the embedded `AimSync.dll` (run as Administrator)

---

## TODO / WIP

- [ ] Re-enable **inventory / skin changer** with a safer apply pipeline (currently stubbed; crashes on in-game apply)
- [ ] Optional sidebar **icon textures** (safe deferred load)
- [ ] Higher-quality **font** without Present-thread file loads that previously crashed
- [ ] Verify / refresh **offsets & patterns** after each CS2 update ([cs2-dumper](https://github.com/a2x/cs2-dumper))

---

## Menu

1. Inject with the loader (or map `AimSync.dll` yourself).
2. Press **INSERT** or **PAUSE** to toggle the overlay.
3. Tabs: **Aim** · **Vis** · **Misc** · **Cfg**

Settings autosave to config (see below).

---

## Build

### Requirements
- Visual Studio 2022+ — Desktop development with C++
- Windows x64, Release

### Commands

```powershell
msbuild Source\AimSync.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild Loader\AimSyncLoader.vcxproj /p:Configuration=Release /p:Platform=x64
```

| Output | Path |
|--------|------|
| Cheat DLL | `x64\Release\AimSync.dll` |
| Auto loader | `x64\Release\AimSync.exe` (embeds the DLL as a resource) |

Rebuild the **loader after the DLL** so the embedded payload stays current.

---

## Inject

1. Start CS2.
2. Run `x64\Release\AimSync.exe` **as Administrator**.
3. Wait for `Injected successfully.`
4. Press **INSERT** in-game / menu.

> Prefer the bundled loader (manual map). Generic LoadLibrary injectors are often blocked or risky.

---

## Config path

| Platform | Path |
|----------|------|
| Windows | `%appdata%\AimSyncCS2\configs\` |

Named configs are `.cfg` files in that folder (e.g. `default.cfg`, `rage.cfg`). The **Cfg** menu tab can create, load, save, and delete them. Autosave still writes the active config.

---

## Project layout

```
AimSync-Internal/
├── Loader/          # Auto injector (manual map + embedded DLL)
├── Source/          # Cheat sources (features, hooks, DX11 UI)
├── Tests/
└── AimSync.sln
```

---

## Disclaimer

For educational / research purposes only. Use at your own risk. You are responsible for compliance with game ToS and any account actions (including bans).

---

## License

MIT — see [LICENSE](LICENSE).

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for branch naming, PR checklist, and review expectations.
Use the pull request template when opening a PR against `main`.
