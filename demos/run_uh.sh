#!/bin/bash
set -euo pipefail

# ── Configuration ────────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly SCRIPT_DIR
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
readonly REPO_ROOT
readonly UH_DIR="$SCRIPT_DIR/unknown-horizons"
readonly UH_REF="${UH_REF:-master}"
readonly VENV_DIR="$REPO_ROOT/.venv-uh"
readonly BUILD_DIR="$REPO_ROOT/out/build/clang22-x64-linux-dbg-cov"
readonly ENGINE_PYTHON_DIR="$REPO_ROOT/src/python"
readonly STAGE_DIR="$REPO_ROOT/out/uh-python"
readonly PYTHON_DEPS=(pyyaml pillow polib distro "greenlet>=3.0")

# ── Utilities ────────────────────────────────────────────────────────────────
log_info()  { echo "→ $*"; }
log_error() { echo "❌ $*" >&2; }

usage() {
  cat <<'EOF'
Usage: demos/run_uh.sh [Unknown Horizons arguments...]

Runs Unknown Horizons against the existing FIFE build in
out/build/clang22-x64-linux-dbg-cov.

Options:
  -h, --help    Show this help.

Environment:
  UH_REF        Unknown Horizons branch or tag to clone (default: master).
  UH_USER_DIR   Overrides the game settings/save directory.
EOF
}

require_build_artifacts() {
  local missing=()
  local artifact

  for artifact in fife.py _fife_swig.so fifechan.py _fifechan_swig.so; do
    if [ ! -f "$BUILD_DIR/$artifact" ]; then
      missing+=("$BUILD_DIR/$artifact")
    fi
  done

  if ! compgen -G "$BUILD_DIR/libfifengine.*" >/dev/null \
      && ! compgen -G "$BUILD_DIR/fifengine.dll" >/dev/null; then
    missing+=("$BUILD_DIR/libfifengine.*")
  fi

  if [ "${#missing[@]}" -ne 0 ]; then
    log_error "FIFE build artifacts are missing:"
    for artifact in "${missing[@]}"; do
      echo "    $artifact" >&2
    done
    cat >&2 <<EOF

Build them first:
  cmake --preset clang22-x64-linux-dbg-cov \\
    -DENABLE_FIFEGUI=ON -DBUILD_PYTHON_BINDINGS=ON -DBUILD_LIBRARY=ON
  cmake --build "$BUILD_DIR" --target fife fife_swig fifechan_swig
EOF
    exit 1
  fi
}

# ── Argument handling ────────────────────────────────────────────────────────
case "${1:-}" in
  -h|--help)
    usage
    exit 0
    ;;
esac

# ── Engine artifacts ─────────────────────────────────────────────────────────
require_build_artifacts
# Stage the source package plus the generated bindings. Copying the generated
# files over src/python/fife would overwrite the tracked fallback fifechan.py.
log_info "Staging FIFE Python bindings in $STAGE_DIR..."
rm -rf "$STAGE_DIR"
mkdir -p "$STAGE_DIR"
cp -a "$ENGINE_PYTHON_DIR/fife" "$STAGE_DIR/fife"
cp -f "$BUILD_DIR/fife.py" "$STAGE_DIR/fife/fife.py"
cp -f "$BUILD_DIR/_fife_swig.so" "$STAGE_DIR/fife/_fife_swig.so"
cp -f "$BUILD_DIR/fifechan.py" "$STAGE_DIR/fife/fifechan.py"
cp -f "$BUILD_DIR/_fifechan_swig.so" "$STAGE_DIR/fife/_fifechan_swig.so"

# ── Unknown Horizons checkout ────────────────────────────────────────────────
if [ ! -f "$UH_DIR/run_uh.py" ]; then
  log_info "Cloning unknown-horizons ($UH_REF)..."
  git clone --depth 1 --branch "$UH_REF" \
    https://github.com/unknown-horizons/unknown-horizons "$UH_DIR"
fi

# ── Python environment ───────────────────────────────────────────────────────
if ! command -v uv &>/dev/null; then
  log_info "Installing uv..."
  python3 -m pip install --break-system-packages uv
  export PATH="$HOME/.local/bin:$PATH"
fi

if [ ! -x "$VENV_DIR/bin/python" ]; then
  log_info "Creating venv at $VENV_DIR..."
  uv venv --python 3.13 "$VENV_DIR"
fi

if ! "$VENV_DIR/bin/python" -c \
    "import yaml, PIL, polib, distro, greenlet" >/dev/null 2>&1; then
  log_info "Installing Python dependencies..."
  uv pip install --python "$VENV_DIR/bin/python" "${PYTHON_DEPS[@]}"
fi

# ── Environment ─────────────────────────────────────────────────────────────
export PYTHONPATH="$STAGE_DIR:${PYTHONPATH:-}"
export LD_LIBRARY_PATH="$BUILD_DIR:$REPO_ROOT/vcpkg_installed/x64-linux/lib:/usr/lib64:$REPO_ROOT/out/fife-dependencies/x64-linux/install/lib:${LD_LIBRARY_PATH:-}"

cd "$UH_DIR"

# ── Game ────────────────────────────────────────────────────────────────────
log_info "Launching Unknown Horizons..."
exec "$VENV_DIR/bin/python" run_uh.py "$@"
