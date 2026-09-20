#!/usr/bin/env bash
# Replace vendored simplewall, routine, and builder with current upstream master.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

clone_latest() {
	local name="$1"
	local url="$2"
	git clone --depth 1 "$url" "$TMP/$name"
	# Drop nested git metadata so this workspace stays a single repo.
	rm -rf "$TMP/$name/.git"
	rm -rf "$ROOT/$name"
	mkdir -p "$ROOT/$name"
	tar -C "$TMP/$name" -cf - . | tar -C "$ROOT/$name" -xf -
}

sha_of() {
	local url="$1"
	git ls-remote "$url" refs/heads/master | awk '{print $1}'
}

echo "Cloning latest henrypp/simplewall, routine, and builder..."
clone_latest simplewall https://github.com/henrypp/simplewall.git
clone_latest routine https://github.com/henrypp/routine.git
clone_latest builder https://github.com/henrypp/builder.git

{
	echo "# Vendored Henry++ trees. Refresh with tools/update-from-upstream.sh"
	printf 'simplewall %s https://github.com/henrypp/simplewall.git\n' "$(sha_of https://github.com/henrypp/simplewall.git)"
	printf 'routine %s https://github.com/henrypp/routine.git\n' "$(sha_of https://github.com/henrypp/routine.git)"
	printf 'builder %s https://github.com/henrypp/builder.git\n' "$(sha_of https://github.com/henrypp/builder.git)"
} > "$ROOT/UPSTREAM"

echo "Updated:"
cat "$ROOT/UPSTREAM"

if [[ ! -f "$ROOT/simplewall/simplewall.sln" || ! -f "$ROOT/routine/src/routine.c" || ! -f "$ROOT/builder/build.bat" ]]; then
	echo "Layout check failed after update." >&2
	exit 1
fi

echo "Layout check OK."
