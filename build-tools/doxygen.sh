#!/usr/bin/env bash

# -----------------------------------------------------------------------------
# Doxygen helper
#
# Purpose
# - This mimics the CI docs workflow locally.
# - Ensure Doxygen, Graphviz and Sphinx are available.
# - Build API docs using `doxygen doxygen.conf`.
#
# Usage
# - ./build-tools/doxygen.sh
#     Checks for required tools and builds docs.
# - ./build-tools/doxygen.sh --install
#     Installs tools with apt-get and latest Doxygen from latest release tag.
# - ./build-tools/doxygen.sh > doxygen.log 2> doxygen-error.log
#
# Notes
# - This script targets Debian/Ubuntu-like systems for package installation.
# - Generated HTML is written to docs/doxygen/html.
# -----------------------------------------------------------------------------

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="${SCRIPT_DIR}/.."
DOXYFILE_PATH="${REPO_ROOT}/doxygen.conf"
OUTPUT_DIR="${REPO_ROOT}/docs/doxygen/html"
INSTALL_TOOLS=0

usage() {
  cat <<'EOF'
Usage: ./build-tools/doxygen.sh [--install]

Options:
  --install    Install missing dependencies and latest Doxygen (requires sudo).
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --install)
      INSTALL_TOOLS=1
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

if [[ ! -f "$DOXYFILE_PATH" ]]; then
  echo "doxygen.conf not found at: $DOXYFILE_PATH"
  exit 1
fi

need_command() {
  local cmd="$1"
  if ! command -v "$cmd" >/dev/null 2>&1; then
    return 1
  fi
  return 0
}

install_apt_packages() {
  if ! need_command apt-get; then
    echo "apt-get is unavailable on this system; install dependencies manually."
    exit 1
  fi

  export DEBIAN_FRONTEND=noninteractive
  sudo apt-get update
  sudo apt-get install -y "$@"
}

install_latest_doxygen() {
  if ! need_command wget || ! need_command jq || ! need_command make; then
    install_apt_packages wget jq make
  fi

  local latest_version
  latest_version="$(wget -qO- "https://api.github.com/repos/doxygen/doxygen/tags" | jq -r '.[0].name | split("_") | join(".")[8:]')"

  if [[ -z "$latest_version" || "$latest_version" == "null" ]]; then
    echo "Failed to resolve latest Doxygen version from GitHub API."
    exit 1
  fi

  local tmp_dir
  tmp_dir="$(mktemp -d)"
  trap 'rm -rf -- "'"$tmp_dir"'"' EXIT

  wget "https://doxygen.nl/files/doxygen-${latest_version}.linux.bin.tar.gz" -O - | tar -xzv --directory="$tmp_dir"

  local extracted_dir
  extracted_dir="$(find "$tmp_dir" -mindepth 1 -maxdepth 1 -type d -name 'doxygen-*' | head -n 1)"
  if [[ -z "$extracted_dir" ]]; then
    echo "Failed to locate extracted Doxygen directory."
    exit 1
  fi

  (cd "$extracted_dir" && sudo make install)
}

if [[ "$INSTALL_TOOLS" -eq 1 ]]; then
  echo "Installing Graphviz and Sphinx packages..."
  install_apt_packages graphviz python3-sphinx python3-sphinx-rtd-theme

  echo "Installing latest Doxygen from upstream tags..."
  install_latest_doxygen
fi

missing=()
for cmd in doxygen dot sphinx-build; do
  if ! need_command "$cmd"; then
    missing+=("$cmd")
  fi
done

if [[ ${#missing[@]} -gt 0 ]]; then
  echo "Missing required tools: ${missing[*]}"
  echo "Run with --install or install tools manually."
  exit 1
fi

echo "Tool versions:"
doxygen -v
dot -V
sphinx-build --version

echo "Building API documentation from ${DOXYFILE_PATH}"
(
  cd "$REPO_ROOT"
  doxygen doxygen.conf
)

if [[ -d "$OUTPUT_DIR" ]]; then
  echo "Documentation built successfully: ${OUTPUT_DIR}"
else
  echo "Doxygen finished, but output directory was not found: ${OUTPUT_DIR}"
  exit 1
fi
