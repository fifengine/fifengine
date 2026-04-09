#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

export PYTHONPATH="$PROJECT_ROOT/src/python:$PYTHONPATH"

cd "$PROJECT_ROOT"

echo "Running swig_tests with pytest..."
uv run pytest tests/swig_tests/test_*.py "$@"
