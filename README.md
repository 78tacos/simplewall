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

On any machine:

```bash
test -f simplewall/simplewall.sln
test -f routine/src/routine.c
test -f builder/build.bat
```

## Build (Windows)

1. Install Visual Studio 2022 or 2026 with the Desktop C++ workload and the Windows 10/11 SDK.
2. Open `simplewall\simplewall.sln`.
3. Restore NuGet (`Microsoft.Windows.CppWinRT` 2.0.230706.1).
4. Build **Release | x64** (or ARM64). Latest `build_vc.bat` looks for VS 2026 and builds those two platforms.

Do not open a copy of `simplewall` by itself. The include path `.\..\routine\src\` only works when `routine` is the sibling folder of `simplewall`.

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
