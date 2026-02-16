#!/usr/bin/env bash

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
python3 -m venv "$VIRTUAL_ENV"
export PATH="$VIRTUAL_ENV/bin:$PATH"

pip install --prefer-binary --no-cache-dir --upgrade pip
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
  pip install --prefer-binary --no-cache-dir "${packages[@]}"
fi

# Paths to check (mirror QA workflow)
PATHS=(
  src/python/fife
  tests/extension_tests
  tests/swig_tests
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
  echo "Running black --check on: ${EXISTING[*]}"
  black --check "${EXISTING[@]}"
fi

if [[ "$run_ruff" == true ]]; then
  echo "Running ruff on: ${EXISTING[*]}"
  ruff check "${EXISTING[@]}"
fi

if [[ "$run_mypy" == true && -d src/python/fife ]]; then
  echo "Running mypy on src/python/fife"
  mypy src/python/fife
fi

echo "Python formatting/lint/type checks complete."
