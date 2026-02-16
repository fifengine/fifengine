#!/usr/bin/env bash

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
