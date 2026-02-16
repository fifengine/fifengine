#!/usr/bin/env bash

set -euo pipefail

CLANG_FORMAT=${CLANG_FORMAT:-}

check_version() {
  local binary="$1"
  if command -v "$binary" >/dev/null 2>&1; then
    local version
    version=$("$binary" --version)
    local major
    major=$(echo "$version" | sed -nE 's/.*version ([0-9]+).*/\1/p')
    if [[ -n "$major" && "$major" -ge 17 ]]; then
      echo "$binary"
      return 0
    fi
  fi
  return 1
}

if [[ -n "$CLANG_FORMAT" ]]; then
  if ! CLANG_FORMAT=$(check_version "$CLANG_FORMAT"); then
    echo "Error: Requested clang-format '$CLANG_FORMAT' is unavailable or unsupported (need >=17)."
    exit 1
  fi
else
  if ! CLANG_FORMAT=$(check_version "clang-format-20") \
    && ! CLANG_FORMAT=$(check_version "clang-format-18") \
    && ! CLANG_FORMAT=$(check_version "clang-format-17") \
    && ! CLANG_FORMAT=$(check_version "clang-format"); then
    echo "Error: No compatible clang-format version (>=17) found."
    exit 1
  fi
fi

echo "Using $($CLANG_FORMAT --version)"

if [[ -z "${CI:-}" && -z "${GITHUB_ACTIONS:-}" ]]; then
  if ! command -v dos2unix >/dev/null 2>&1; then
    if command -v apt-get >/dev/null 2>&1; then
      echo "dos2unix not found, installing via apt-get..."
      sudo apt-get update
      sudo apt-get install -y dos2unix
    else
      echo "Error: dos2unix not found and apt-get is unavailable."
      exit 1
    fi
  fi
  find src tests -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.cpp" \) -exec dos2unix {} \;
fi

find src tests -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.cpp" \) -exec "$CLANG_FORMAT" -i -style=file {} \;

if [[ -n "${CI:-}" || -n "${GITHUB_ACTIONS:-}" ]]; then
  if ! git -c core.fileMode=false diff --exit-code; then
    echo "Error: Code formatting issues detected. Please run ./build-tools/format.sh and commit the changes."
    exit 1
  fi
fi
