#!/usr/bin/env bash
# Verify the sibling layout Henry++'s project files expect.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

missing=0
require() {
	local path="$1"
	if [[ ! -e "$path" ]]; then
		echo "missing: $path" >&2
		missing=1
	fi
}

require simplewall/simplewall.sln
require simplewall/simplewall.vcxproj
require simplewall/packages.config
require simplewall/src/main.c
require simplewall/src/wfp.c
require simplewall/src/resource.rc
require builder/build.bat
require builder/build_locale.bat
require builder/src/build_package.py

# Paths resolved the same way simplewall.vcxproj does (from simplewall/).
require simplewall/../routine/src/routine.c
require simplewall/../routine/src/rapp.c
require simplewall/../routine/src/routine.h
require simplewall/../routine/src/rapp.h
require simplewall/../routine/src/rconfig.h
require simplewall/../routine/src/ntapi.h
require simplewall/../routine/src/ntrtl.h
require simplewall/../routine/src/rtypes.h

if [[ "$missing" -ne 0 ]]; then
	echo "Workspace layout is incomplete. simplewall, routine, and builder must be siblings." >&2
	exit 1
fi

echo "Workspace layout is OK."
echo "  simplewall/  app (open simplewall.sln)"
echo "  routine/     compiler SDK used via ../routine"
echo "  builder/     packaging scripts used via ../builder"
