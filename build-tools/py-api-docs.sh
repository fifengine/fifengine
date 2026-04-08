#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SPHINX_DIR="${SCRIPT_DIR}/../docs/sphinx"

cd "${SPHINX_DIR}"
uv run --group docs sphinx-build -b html . html
