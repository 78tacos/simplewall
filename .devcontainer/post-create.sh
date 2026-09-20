#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

bash tools/check-layout.sh

cat <<'EOF'

Codespaces note
---------------
This is a Linux editor environment. simplewall is a Win32 / WFP app and
will not link here (no MSVC, no Windows SDK, no fwpuclnt).

Compile it with:
  * GitHub Actions  →  gh workflow run Build
    then            →  gh run watch
    artifact        →  simplewall-x64 (simplewall.exe)
  * A Windows machine with Visual Studio 2026 (toolset v145)
    open simplewall\simplewall.sln, restore NuGet, build Release | x64.

EOF
