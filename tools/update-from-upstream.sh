#!/usr/bin/env bash
# Replace vendored simplewall, routine, and builder with current upstream master.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

clone_latest() {
	local name="$1"
	local url="$2"
	local sha_var="$3"
	git clone --depth 1 "$url" "$TMP/$name"
	# Record the cloned commit before dropping nested git metadata.
	printf -v "$sha_var" '%s' "$(git -C "$TMP/$name" rev-parse HEAD)"
	rm -rf "$TMP/$name/.git"
	rm -rf "$ROOT/$name"
	mkdir -p "$ROOT/$name"
	tar -C "$TMP/$name" -cf - . | tar -C "$ROOT/$name" -xf -
}

echo "Cloning latest henrypp/simplewall, routine, and builder..."
clone_latest simplewall https://github.com/henrypp/simplewall.git SIMPLEWALL_SHA
clone_latest routine https://github.com/henrypp/routine.git ROUTINE_SHA
clone_latest builder https://github.com/henrypp/builder.git BUILDER_SHA

{
	echo "# Vendored Henry++ trees. Refresh with tools/update-from-upstream.sh"
	printf 'simplewall %s https://github.com/henrypp/simplewall.git\n' "$SIMPLEWALL_SHA"
	printf 'routine %s https://github.com/henrypp/routine.git\n' "$ROUTINE_SHA"
	printf 'builder %s https://github.com/henrypp/builder.git\n' "$BUILDER_SHA"
} > "$ROOT/UPSTREAM"

echo "Updated:"
cat "$ROOT/UPSTREAM"

bash "$ROOT/tools/check-layout.sh"
