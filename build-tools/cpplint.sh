#!/usr/bin/env bash

# --------------------------------------------------------------------------
# CPPLINT helper
#
# This script sets up a virtual environment and installs cpplint, then runs
# cpplint on the specified directories.
#
# Usage:
# -  ./build-tools/cpplint.sh
#   Checks for required tools and runs cpplint on the specified directories.
# - ./build-tools/cpplint.sh > cpplint.log 2> cpplint-todo.log
#
# Notes
# - This script targets Debian/Ubuntu-like systems for package installation.
# -----------------------------------------------------------------------------

set -euo pipefail

if ! command -v python3 >/dev/null 2>&1; then
  echo "python3 is required"
  exit 1
fi

VIRTUAL_ENV=${VIRTUAL_ENV:-/tmp/fife-cpplint-venv}
python3 -m venv "$VIRTUAL_ENV"
export PATH="$VIRTUAL_ENV/bin:$PATH"

pip install --prefer-binary --no-cache-dir --upgrade pip
pip install --prefer-binary --no-cache-dir cpplint

cpplint --recursive src tests/core_tests
