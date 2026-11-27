#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
#
# Local GLCore regression run with a real software GLES2/GL context.
# Requires Xvfb, Mesa llvmpipe and EGL/GLES libraries.

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_ES2="${1:-$ROOT/out/build/glcore-es2}"
BUILD_CORE="${2:-$ROOT/out/build/glcore-core}"

export LIBGL_ALWAYS_SOFTWARE=true
export GALLIUM_DRIVER=llvmpipe
export EGL_PLATFORM=x11
export FIFE_GL_REQUIRE_CONTEXT=1

run() {
    local build="$1"
    echo "== GLCore tests: $build =="
    xvfb-run -a -s "-screen 0 1280x800x24" \
        "$build/tests/core_tests/all_tests"
}

run "$BUILD_ES2"
run "$BUILD_CORE"
