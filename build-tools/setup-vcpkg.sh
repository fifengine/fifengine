#!/usr/bin/env bash

set -euo pipefail

# Remember the original dir
START_DIR="$(pwd)"

# ------------------------------------------------------------------
# Install additional components
# ------------------------------------------------------------------

apt-get update

# Package required to build libxcrypt
apt-get install -y autoconf autoconf-archive automake libtool

# Packages required to build SDL3
# See https://wiki.libsdl.org/SDL3/README-linux#build-dependencies

apt-get install -y build-essential git make \
  pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
  libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
  libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
  libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
  libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev \
  libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev \
  libglu1-mesa-dev

apt-get install -y libx11-dev libxft-dev libxext-dev

# Packages required to build for Glew

apt-get install -y libgl1-mesa-dev libglu1-mesa-dev

# Packages required to build SWIG bindings

apt-get install -y python3 python3.13-dev

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
