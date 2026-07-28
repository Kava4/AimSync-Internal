# Contributing to AimSync Internal

Thanks for contributing. This project targets **Windows x64** only (DX11 overlay + loader).

## Before you start

1. Open an issue for larger changes (new features, refactors, offset overhauls).
2. Keep PRs focused — one concern per PR when possible.
3. Do **not** commit build outputs (`x64/`, `obj/`), secrets, or personal configs.
4. Do **not** vendor third-party cheat sources into the public tree (`examples_for_impl/` is gitignored for local reference only).

## Development setup

- Visual Studio 2022+ with Desktop C++ workload
- Build:

```powershell
msbuild Source\AimSync.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild Loader\AimSyncLoader.vcxproj /p:Configuration=Release /p:Platform=x64
```

- CI runs the Windows workflow on push / PR (`msbuild` + `cmake`, Debug & Release).

## Pull request process

### 1. Branch

```powershell
git checkout main
git pull origin main
git checkout -b feature/short-description
```

Use prefixes like `feature/`, `fix/`, `chore/`, `docs/`.

### 2. Make changes

- Match existing style (headers, `noexcept`, config variables, HookContext patterns).
- Prefer small, reviewable diffs.
- Update `README.md` Status / TODO when you change user-facing behavior.
- After CS2 patches, document which offsets/patterns you touched.

### 3. Test locally

- [ ] Release build of `AimSync.dll` succeeds
- [ ] Loader builds if you touched `Loader/`
- [ ] Inject + smoke-test the menu (INSERT) if UI/features changed
- [ ] Confirm unload still works if you touched hooks / Present

### 4. Open the PR

```powershell
git push -u origin HEAD
gh pr create
```

Or open a PR on GitHub against **`main`**.

Fill in the PR template:

- **Summary** — what and why
- **Test plan** — how reviewers can verify
- Link related issues (`Fixes #123`)

### 5. Review expectations

- Keep the PR green on the **Windows** CI badge.
- Respond to review comments with new commits (prefer not force-pushing shared branches unless asked).
- Maintainers may squash-merge.

## Versioning & releases

- Tags follow `vMAJOR.MINOR.PATCH` (e.g. `v1.2.0`).
- GitHub Releases are cut from tags on `main`.
- Do not create release tags on feature branches.

## Code of conduct (short)

Be respectful. No harassment. Discuss technical disagreements on the PR / issue, not personally.
