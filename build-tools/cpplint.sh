#!/usr/bin/env bash

# -----------------------------------------------------------------------------
# CPPLINT helper
#
# Purpose
# - This mimics the CI linting workflow locally.
# - Setup Python virtual environment using uv.
# - Run cpplint on C++ source files.
#
# Usage
# - ./build-tools/cpplint.sh
#     Checks for required tools and runs cpplint on C++ directories.
# - ./build-tools/cpplint.sh > cpplint.log 2> cpplint-error.log
#
# Notes
# - This script uses uv for Python package management.
# - Checks src/ and tests/core_tests/ directories by default.
# -----------------------------------------------------------------------------

set -euo pipefail

VIRTUAL_ENV=${VIRTUAL_ENV:-/tmp/fife-cpplint-venv}

if ! command -v uv >/dev/null 2>&1; then
  echo "Installing uv..."
  pip install uv
fi

if [[ -d "$VIRTUAL_ENV" ]]; then
  echo "Using existing virtual environment: $VIRTUAL_ENV"
else
  echo "Creating virtual environment: $VIRTUAL_ENV"
  uv venv "$VIRTUAL_ENV"
fi
export PATH="$VIRTUAL_ENV/bin:$PATH"

uv pip install cpplint

cpplint --recursive src tests/core_tests
