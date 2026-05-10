#!/bin/bash
set -euo pipefail

# ── Configuration ────────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly SCRIPT_DIR
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
readonly REPO_ROOT
readonly INNER_SCRIPT="$SCRIPT_DIR/unknown-horizons/run_uh.sh"
readonly VENV_DIR="$REPO_ROOT/.venv-uh"
readonly WHEEL_DIR="$REPO_ROOT/.cache/local-wheels"
readonly REQUIRED_LIBS=(libopenal.so.1 libSDL3.so.0 libSDL3_image.so.0 libSDL3_ttf.so.0)
readonly PYTHON_RUNTIME_DEPS=(pyyaml pillow polib distro "greenlet>=3.0")

# ── Utilities ────────────────────────────────────────────────────────────────
log_info()  { echo "→ $*"; }
log_warn()  { echo "⚠️ $*" >&2; }
log_error() { echo "❌ $*" >&2; }

ensure_path() {
  export PATH="$HOME/.local/bin:$PATH"
}

# ── Repository & Tooling Setup ───────────────────────────────────────────────
clone_unknown_horizons() {
  [[ -f "$INNER_SCRIPT" ]] && return 0
  log_info "Cloning unknown-horizons..."
  git clone --depth 1 https://github.com/unknown-horizons/unknown-horizons \
    "$SCRIPT_DIR/unknown-horizons"
}

install_uv() {
  command -v uv &>/dev/null && return 0
  log_info "Installing uv..."

  if command -v pipx &>/dev/null; then
    pipx install uv
  else
    python3 -m pip install --break-system-packages uv
  fi

  ensure_path
  command -v uv &>/dev/null || {
    log_error "uv not available after installation"
    exit 1
  }
}

# ─── Dependencies ────────────────────────────────────────────────────────────
check_library_installed() {
  local lib="$1"
  ldconfig -p 2>/dev/null | grep -q "$lib"
}

get_missing_libraries() {
  local -a missing=()
  for lib in "${REQUIRED_LIBS[@]}"; do
    check_library_installed "$lib" || missing+=("$lib")
  done
  echo "${missing[@]}"
}

install_system_deps() {
  command -v apt-get &>/dev/null || return 1
  log_info "Installing system dependencies..."
  apt-get update -qq
  DEBIAN_FRONTEND=noninteractive apt-get install -y -qq \
    libopenal1 libsdl3-0 libsdl3-image0 libsdl3-ttf0 patchelf
}

check_runtime_deps() {
  local -a missing
  read -ra missing <<< "$(get_missing_libraries)"
  (( ${#missing[@]} )) || return 0

  log_warn "Missing libraries: ${missing[*]}"
  install_system_deps && return 0

  log_error "Cannot install deps. Please install OpenAL+SDL3 manually."
  exit 1
}

# ── Python Build Dependencies ────────────────────────────────────────────────
ensure_build_deps() {
  python3 -c "import scikit_build_core, packaging" 2>/dev/null && return 0
  log_info "Installing Python build dependencies..."
  uv pip install --break-system-packages scikit_build_core packaging
}

# ── Wheel Building & Repair ────────────────────────────────────────────────
build_libfife_wheel() {
  local output_dir="$1"
  mkdir -p "$output_dir"

  log_info "Building libfife wheel..."
  uv build "$REPO_ROOT" --wheel -o "$output_dir" --no-build-isolation \
    -Ccmake.define.BUILD_SHARED_LIBS=ON \
    -Ccmake.define.BUILD_TESTS=OFF
}

find_latest_wheel() {
  local pattern="$1"
  shopt -s nullglob
  local -a wheels
  # shellcheck disable=SC2086,SC2206
  wheels=($pattern)
  shopt -u nullglob
  (( ${#wheels[@]} )) && echo "${wheels[0]}"
}

repair_wheel_with_auditwheel() {
  local wheel_path="$1"
  local output_dir="$2"

  command -v auditwheel &>/dev/null || \
    uv pip install --break-system-packages auditwheel >/dev/null
  command -v auditwheel &>/dev/null || return 1

  log_info "Repairing wheel with auditwheel..."
  auditwheel repair "$wheel_path" -w "$output_dir" 2>/dev/null
}

# ── Python Virtual Environment & Installation ───────────────────────────────
setup_venv() {
  local venv_path="$1"
  log_info "Creating virtual environment at $venv_path..."
  rm -rf "$venv_path"
  uv venv "$venv_path"
}

install_local_wheel() {
  local python_path="$1"
  local wheel_path="$2"
  local wheel_dirs=("${@:3}")

  local find_links_args=()
  for dir in "${wheel_dirs[@]}"; do
    find_links_args+=(--find-links "$dir")
  done

  uv pip install --python "$python_path" \
    "${find_links_args[@]}" --no-index --force-reinstall "$wheel_path"
}

install_runtime_python_deps() {
  local python_path="$1"
  log_info "Installing runtime Python dependencies..."
  uv pip install --python "$python_path" "${PYTHON_RUNTIME_DEPS[@]}"
}

# ── Build & Install Fifengine ────────────────────────────────────────────────────────────────
build_and_install_libfife() {
  local tmp_dir="$WHEEL_DIR/tmp"
  local repaired_dir="$WHEEL_DIR/repaired"
  mkdir -p "$tmp_dir" "$repaired_dir"
  rm -f "$tmp_dir"/libfife-*.whl "$repaired_dir"/libfife-*.whl

  ensure_build_deps
  build_libfife_wheel "$tmp_dir"

  local wheel_path
  wheel_path="$(find_latest_wheel "$tmp_dir/libfife-*.whl")"
  [[ -n "$wheel_path" ]] || { log_error "No wheel built"; exit 1; }

  local install_wheel="$wheel_path"
  if repair_wheel_with_auditwheel "$wheel_path" "$repaired_dir"; then
    local repaired
    repaired="$(find_latest_wheel "$repaired_dir/libfife-*.whl")"
    [[ -n "$repaired" ]] && install_wheel="$repaired"
  else
    log_warn "auditwheel repair failed, using unrepaired wheel"
  fi

  setup_venv "$VENV_DIR"
  local python_bin="$VENV_DIR/bin/python"

  install_local_wheel "$python_bin" "$install_wheel" "$repaired_dir" "$tmp_dir"
  install_runtime_python_deps "$python_bin"

  export FIFE_UH_PYTHON="$python_bin"
}

# ── Main ────────────────────────────────────────────────────────────────
main() {
  ensure_path
  clone_unknown_horizons
  install_uv
  check_runtime_deps
  build_and_install_libfife

  log_info "Launching Unknown Horizons..."
  exec "$INNER_SCRIPT" "$@"
}

# ── Entry Point ────────────────────────────────────────────────────────────────
main "$@"
