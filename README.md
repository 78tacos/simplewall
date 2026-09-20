# simplewall workspace

This repository vendors Henry++’s three sibling projects so a clone is actually buildable.

Upstream [simplewall](https://github.com/henrypp/simplewall) does not contain its compiler SDK or packaging scripts. Those live in separate repos and are expected **next to** the app, not inside it:

```text
workspace/
  simplewall/   Windows Filtering Platform firewall (app)
  routine/      shared Win32 “project SDK” (required to compile)
  builder/      locale, rules, installer, and portable-package scripts
```

`simplewall.vcxproj` already compiles `..\routine\src\routine.c` and `..\routine\src\rapp.c`. The `build*.bat` scripts already `cd ..\builder`. This layout matches that.

The public `routine` repo and current simplewall are not the same private SDK revision Henry++ builds with. `simplewall/src/include/routine-compat.h` maps the published helpers (section-aware `*_ex` config APIs, `PCR_STRINGREF`, and a few renamed calls) so the tree can compile.

## What is pinned

| Directory | Upstream | Version / commit |
|---|---|---|
| `simplewall/` | [henrypp/simplewall](https://github.com/henrypp/simplewall) | **3.9.1** (`482893bcd8fca69c419795cd169dba4c09ef2de9`, 2026-09-18) |
| `routine/` | [henrypp/routine](https://github.com/henrypp/routine) | `1acd9394395b7cee9e1ea7b985fb54e76be86b11` (2026-09-11) |
| `builder/` | [henrypp/builder](https://github.com/henrypp/builder) | `74d1faba0a75feda417313909c96b256fae7e79f` (2026-07-23) |

This used to be a lone 3.7.5 snapshot. That tree could not compile without the two missing siblings.

## Setup check

On Windows:

```bat
setup.bat
```

On Linux, macOS, or a Codespace:

```bash
bash tools/check-layout.sh
```

## Build (Windows)

simplewall is a Win32 WFP app. It needs **MSVC toolset v145 (Visual Studio 2026)** and the Windows SDK. It will not compile on Linux or in a GitHub Codespace.

1. Install Visual Studio 2026 with the Desktop C++ workload and the Windows 10/11 SDK.
2. Open `simplewall\simplewall.sln`.
3. Restore NuGet (`Microsoft.Windows.CppWinRT` 2.0.230706.1).
4. Build **Release | x64** (or ARM64). `build_vc.bat` looks for VS 2026.

Do not open a copy of `simplewall` by itself. The include path `.\..\routine\src\` only works when `routine` is the sibling folder of `simplewall`.

### GitHub Actions

The **Build** workflow (`.github/workflows/build.yml`) checks the sibling layout on Ubuntu, then compiles **Release | x64** on `windows-latest` (Visual Studio 2026). The `simplewall.exe` artifact is named `simplewall-x64`.

```bash
gh workflow run Build
gh run watch --exit-status
```

### GitHub Codespaces

[Open in GitHub Codespaces](https://codespaces.new/78tacos/simplewall)

The devcontainer (`.devcontainer/devcontainer.json`) is a Linux C++ editor: C/C++ IntelliSense, include paths into `simplewall/src` and `routine/src`, and GitHub CLI. After create it runs `tools/check-layout.sh`. Use it to browse and edit; trigger the Windows CI workflow to compile.

## Packaging and locales

From `simplewall\`:

- `build.bat` — portable/installer package via `..\builder` (needs Python 3.13+, 7-Zip, GPG, NSIS)
- `build_locale.bat` — rebuild `bin\simplewall.lng` from `bin\i18n\*.ini`
- `build_simplewall_rules.bat` — refresh the internal blocklist

## Refresh from upstream

```bash
./tools/update-from-upstream.sh
```

That replaces `simplewall/`, `routine/`, and `builder/` with a fresh `--depth 1` clone of each `master` and records the new SHAs in `UPSTREAM`.
