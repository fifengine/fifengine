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

if ! command -v python3 >/dev/null 2>&1; then
  echo "python3 is required"
  exit 1
fi

# Default: run all tools. If any of --black/--ruff/--mypy are provided,
# only run the requested ones.
run_black=false
run_ruff=false
run_mypy=false
flags_provided=false

usage() {
  cat <<EOF
Usage: $0 [--black] [--ruff] [--mypy] [--help]

If no flags are provided, all tools are run.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --black)
      run_black=true
      flags_provided=true
      shift
      ;;
    --ruff)
      run_ruff=true
      flags_provided=true
      shift
      ;;
    --mypy)
      run_mypy=true
      flags_provided=true
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      usage
      exit 1
      ;;
  esac
done

# If no flags were provided, enable all tools
if [[ "$flags_provided" != true ]]; then
  run_black=true
  run_ruff=true
  run_mypy=true
fi

VIRTUAL_ENV=${VIRTUAL_ENV:-/tmp/fife-python-venv}

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

run_in_venv() {
  printf 'Running:'
  for arg in "$VIRTUAL_ENV/bin/python" -m "$@"; do
    printf ' %q' "$arg"
  done
  printf '\n'
  "$VIRTUAL_ENV/bin/python" -m "$@"
}

packages=()
if [[ "$run_black" == true ]]; then
  packages+=(black)
fi
if [[ "$run_ruff" == true ]]; then
  packages+=(ruff)
fi
if [[ "$run_mypy" == true ]]; then
  packages+=(mypy)
fi

if [[ ${#packages[@]} -gt 0 ]]; then
  uv pip install "${packages[@]}"
fi

# Paths to check (mirror QA workflow)
PATHS=(
  src/python/fife
  tests/extension_tests
  tests/swig_tests
  demos/cegui
  demos/pychan
  demos/rio_de_hola
  demos/rpg
  demos/shooter
  run_tests.py
)

EXISTING=()
for p in "${PATHS[@]}"; do
  if [[ -e "$p" ]]; then
    EXISTING+=("$p")
  fi
done

if [[ ${#EXISTING[@]} -eq 0 ]]; then
  echo "No target paths found; nothing to check."
  exit 0
fi

if [[ "$run_black" == true ]]; then
  run_in_venv black "${EXISTING[@]}"
fi

if [[ "$run_ruff" == true ]]; then
  run_in_venv ruff check "${EXISTING[@]}"
fi

if [[ "$run_mypy" == true && -d src/python/fife ]]; then
  run_in_venv mypy src/python/fife
fi

echo "Python formatting/lint/type checks complete."
