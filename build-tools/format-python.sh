#!/usr/bin/env bash

# -----------------------------------------------------------------------------
# Python formatting/linting/type checking helper
#
# Purpose
# - This mimics the CI QA workflow locally.
# - Setup Python virtual environment using uv.
# - Run black, ruff, and/or mypy on Python source files.
#
# Usage
# - ./build-tools/format-python.sh
#     Run all tools (black, ruff, mypy).
# - ./build-tools/format-python.sh --black > black.log
#     Run only black formatting.
# - ./build-tools/format-python.sh --ruff > ruff.log
#     Run only ruff linting.
# - ./build-tools/format-python.sh --mypy > mypy.log
#     Run only mypy type checking.
# - ./build-tools/format-python.sh --black --ruff
#     Run specific tools only.
#
# Notes
# - This script uses uv for Python package management.
# - Checks src/python/fife and various demos/tests directories.
# -----------------------------------------------------------------------------

set -euo pipefail

run_black=false
run_ruff=false
run_mypy=false
flags_provided=false

while [[ $# -gt 0 ]]; do
  case "$1" in
    --black) run_black=true; flags_provided=true; shift ;;
    --ruff) run_ruff=true; flags_provided=true; shift ;;
    --mypy) run_mypy=true; flags_provided=true; shift ;;
    --help|-h) echo "Usage: $0 [--black] [--ruff] [--mypy]"; exit 0 ;;
    *) echo "Unknown: $1"; exit 1 ;;
  esac
done

[[ "$flags_provided" != true ]] && run_black=true run_ruff=true run_mypy=true

PATHS=(
  src/python/fife
  tests/extension_tests
  tests/swig_tests
  demos/pychan
  demos/rio_de_hola
  demos/rpg
  demos/shooter
  run_tests.py
)

EXISTING=(${PATHS[@]/#\/workspaces\/fifengine\/})

[[ "$run_ruff" == true ]] && uv run --group dev ruff check "${EXISTING[@]}"
[[ "$run_black" == true ]] && uv run --group dev black "${EXISTING[@]}"
[[ "$run_mypy" == true ]] && uv run --group dev mypy src/python/fife
