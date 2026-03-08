#!/usr/bin/env bash

# Exit on errors, unset vars, and pipe failures
set -euo pipefail

# Remember the original dir
START_DIR="$(pwd)"

# ------------------------------------------------------------------
# Install additional components
# ------------------------------------------------------------------

apt-get update

# Package required to build libxcrypt
apt-get install -y autoconf autoconf-archive automake libtool

# Packages required to build SDL2

apt-get install -y libx11-dev libxft-dev libxext-dev

# Install jq if not already installed (required to update vcpkg baseline)
if ! command -v jq > /dev/null 2>&1; then
	apt-get install -y jq
fi

# ------------------------------------------------------------------
# SETUP VCPKG
# -------------------------------------------------------------------

cd /opt/vcpkg || exit 1

git fetch --tags
LATEST_VCPKG_TAG="$(git tag --sort=-creatordate | head -n 1)"
git checkout "$LATEST_VCPKG_TAG"

./bootstrap-vcpkg.sh

vcpkg integrate install

# Update VCPKG baseline in vcpkg.json

if [ -f "$START_DIR/vcpkg.json" ]; then
	BASELINE="$(git -C /opt/vcpkg rev-parse "${LATEST_VCPKG_TAG}^{commit}")"
	tmpfile="$(mktemp)"
	jq --arg baseline "$BASELINE" '. + {"builtin-baseline": $baseline}' "$START_DIR/vcpkg.json" > "$tmpfile"
	mv "$tmpfile" "$START_DIR/vcpkg.json"

	UPDATED_BASELINE="$(jq -r '.["builtin-baseline"] // empty' "$START_DIR/vcpkg.json")"
	if [ -z "$UPDATED_BASELINE" ]; then
		echo "Error: builtin-baseline missing in $START_DIR/vcpkg.json after update." >&2
		exit 1
	fi

	if [ "$UPDATED_BASELINE" != "$BASELINE" ]; then
		echo "Error: builtin-baseline ($UPDATED_BASELINE) does not match latest tag commit ($BASELINE)." >&2
		exit 1
	fi

	echo "vcpkg.json baseline was updated to latest tag $LATEST_VCPKG_TAG commit: $BASELINE"
fi

# Return to the original working dir
cd "$START_DIR" || exit 1
