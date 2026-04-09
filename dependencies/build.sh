#!/usr/bin/env bash

# This script is for building and installing the "non-vcpkg" FIFE dependencies.
# Its for use with the .devcontainer.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BUILD_DIR="$SCRIPT_DIR/../out/fife-dependencies/x64-linux/build"
INSTALL_DIR="$SCRIPT_DIR/../out/fife-dependencies/x64-linux/install"
VCPKG_DIR="$SCRIPT_DIR/../vcpkg_installed"

cmake -S "$SCRIPT_DIR" \
      -B "$BUILD_DIR" \
      -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DVCPKG_TARGET_TRIPLET=x64-linux \
      -DVCPKG_INSTALLED_DIR="$VCPKG_DIR" \
      -DCMAKE_PREFIX_PATH="$VCPKG_DIR"

cmake --build "$BUILD_DIR"

ls -la "$INSTALL_DIR/lib/cmake/fifechan/" | grep "fifechan"
