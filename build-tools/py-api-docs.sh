#!/usr/bin/env bash

# -----------------------------------------------------------------------------
# Python API Documentation helper using Sphinx
#
# Purpose
# - Setup Python environment for building Python API docs with Sphinx
# - Build HTML documentation from docs/sphinx/ directory
#
# Usage
# - ./build-tools/py-api-docs.sh
#     Setup environment and build docs
# - ./build-tools/py-api-docs.sh --install-deps
#     Only install dependencies
# -----------------------------------------------------------------------------

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="${SCRIPT_DIR}/.."
SPHINX_DIR="${REPO_ROOT}/docs/sphinx"
CONF_FILE="${SPHINX_DIR}/conf.py"
VENV_DIR="${REPO_ROOT}/.venv-sphinx"
INSTALL_DEPS_ONLY=0

usage() {
  cat <<'EOF'
Usage: ./build-tools/py-api-docs.sh [options]

Options:
  --install-deps    Only install dependencies, do not build
  -h, --help       Show this help message
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --install-deps)
      INSTALL_DEPS_ONLY=1
      shift
      ;;
    -h|--help)
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

setup_venv() {
  if ! command -v uv >/dev/null 2>&1; then
    echo "Installing uv..."
    pip install uv
  fi

  if [[ -d "$VENV_DIR" ]]; then
    echo "Using existing virtual environment: $VENV_DIR"
  else
    echo "Creating virtual environment: $VENV_DIR"
    uv venv "$VENV_DIR"
  fi

  export PATH="$VENV_DIR/bin:$PATH"

  echo "Installing dependencies..."

  local packages=("sphinx>=7.0" "furo>=2024.1" "sphinx-autodoc-typehints" "myst-parser")

  if [[ -f "${SPHINX_DIR}/requirements.txt" ]]; then
    uv pip install -r "${SPHINX_DIR}/requirements.txt" "${packages[@]}"
  else
    uv pip install "${packages[@]}"
  fi
}

build_docs() {
  if [[ ! -f "$CONF_FILE" ]]; then
    echo "Sphinx configuration not found at: $CONF_FILE"
    exit 1
  fi

  if [[ ! -d "${SPHINX_DIR}" ]]; then
    echo "Sphinx source directory not found: $SPHINX_DIR"
    exit 1
  fi

  local output_dir="${SPHINX_DIR}/html"
  echo "Building Python API documentation..."

  export PATH="$VENV_DIR/bin:$PATH"

  cd "${SPHINX_DIR}"
  "$VENV_DIR/bin/sphinx-build" -b html . "${output_dir}"
  cd - >/dev/null

  if [[ -d "$output_dir" ]]; then
    echo "Documentation built successfully: ${output_dir}"
  else
    echo "Build finished, but output directory was not found: ${output_dir}"
    exit 1
  fi
}

echo "Setting up Python environment for Sphinx..."
setup_venv

if [[ "$INSTALL_DEPS_ONLY" -eq 1 ]]; then
  echo "Dependencies installed. Run without --install-deps to build docs."
  exit 0
fi

build_docs
