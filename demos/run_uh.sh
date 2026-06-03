#!/bin/bash
set -euo pipefail

# ── Configuration ────────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly SCRIPT_DIR
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
readonly REPO_ROOT
readonly UH_DIR="$SCRIPT_DIR/unknown-horizons"
readonly VENV_DIR="$REPO_ROOT/.venv-uh"
readonly BUILD_DIR="$REPO_ROOT/out/build/clang22-x64-linux-dbg-cov"
readonly ENGINE_PYTHON_DIR="$REPO_ROOT/src/python"
readonly PYTHON_RUNTIME_DEPS=(pyyaml pillow polib distro "greenlet>=3.0")
# ── Utilities ────────────────────────────────────────────────────────────────
log_info()  { echo "→ $*"; }
log_warn()  { echo "⚠️ $*" >&2; }
log_error() { echo "❌ $*" >&2; }


# Clone UH if not present
if [ ! -f "$UH_DIR/run_uh.py" ]; then
  log_info "Cloning unknown-horizons..."
  git clone --depth 1 https://github.com/unknown-horizons/unknown-horizons "$UH_DIR"
fi

# Ensure uv is available
if ! command -v uv &>/dev/null; then
  log_info "Installing uv..."
  python3 -m pip install --break-system-packages uv
  export PATH="$HOME/.local/bin:$PATH"
fi

# Create venv and install UH runtime deps
if [ ! -f "$VENV_DIR/bin/python" ]; then
  log_info "Creating venv at $VENV_DIR..."
  uv venv "$VENV_DIR"
fi

log_info "Installing runtime Python dependencies..."
uv pip install --python "$VENV_DIR/bin/python" "${PYTHON_RUNTIME_DEPS[@]}"

# Use existing build like other demos
export PYTHONPATH="$ENGINE_PYTHON_DIR:${PYTHONPATH:-}"
export LD_LIBRARY_PATH="$BUILD_DIR:$REPO_ROOT/vcpkg_installed/x64-linux/lib:/usr/lib64:$REPO_ROOT/out/fife-dependencies/x64-linux/install/lib:${LD_LIBRARY_PATH:-}"

cd "$UH_DIR"
export FIFE_UH_PYTHON="$VENV_DIR/bin/python"

log_info "Launching Unknown Horizons..."
exec "$VENV_DIR/bin/python" run_uh.py "$@"
