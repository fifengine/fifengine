#!/usr/bin/env bash

# This script is for building and installing the "non-vcpkg" FIFE dependencies.
# Its for use with the .devcontainer.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cmake -S "$SCRIPT_DIR" \
      -B "$SCRIPT_DIR/../out/fife-dependencies/build" \
      -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DVCPKG_TARGET_TRIPLET=x64-linux \
      -DCMAKE_PREFIX_PATH=/opt/vcpkg/installed/x64-linux

cmake --build "$SCRIPT_DIR/../out/fife-dependencies/build"

ls -la "$SCRIPT_DIR/../out/fife-dependencies/install/lib/cmake/fifechan/" | grep "fifechan"
