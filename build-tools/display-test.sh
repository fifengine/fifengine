#!/usr/bin/env bash
set -euo pipefail

# ==============================================================================
# display-test.sh — Display diagnostic tests for devcontainers
# ==============================================================================

APP="${APP:-xeyes}"
MODE="run"          # run, debug, check
VERBOSE=0
FORCE_INSTALL=0

usage() {
  cat <<'EOF'
Usage: display-test.sh [OPTIONS]
  --app APP        App/command to launch (xeyes|xclock|glxgears|custom) [default: xeyes]
  --debug          Print full diagnostics and exit
  --check          Verify display setup only (no install/launch)
  --install        Force package installation
  --verbose        Show detailed output during install/debug
  -h, --help       Show this help
EOF
}

log()   { echo "[display-test] $*"; }
warn()  { echo "[display-test] ⚠️  $*" >&2; }
err()   { echo "[display-test] ❌ $*" >&2; exit 1; }

# ── CLI Parsing ──────────────────────────────────────────────────────────────
parse_args() {
  while [[ $# -gt 0 ]]; do
    case "$1" in
      --app)     APP="$2"; shift 2 ;;
      --debug)   MODE="debug"; shift ;;
      --check)   MODE="check"; shift ;;
      --install) FORCE_INSTALL=1; shift ;;
      --verbose) VERBOSE=1; shift ;;
      -h|--help) usage; exit 0 ;;
      *) err "Unknown option: $1" ;;
    esac
  done
}

# ── Auto-Install ─────────────────────────────────────────────────────────────
ensure_deps() {
  local bins=("xeyes" "glxgears" "xdpyinfo" "glxinfo" "xhost" "xauth")
  local missing=()
  for bin in "${bins[@]}"; do
    command -v "$bin" >/dev/null || missing+=("$bin")
  done

  if [[ ${#missing[@]} -eq 0 && $FORCE_INSTALL -eq 0 ]]; then
    return 0
  fi

  [[ $EUID -ne 0 ]] && err "Root required for package installation. Re-run with sudo or as root."

  log "Installing dependencies... (${#missing[@]} missing)"
  local pkg_list="x11-apps mesa-utils x11-utils x11-xserver-utils xauth libgl1-mesa-dri"
  if [[ $VERBOSE -eq 0 ]]; then
    apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq "$pkg_list" >/dev/null 2>&1
  else
    apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y "$pkg_list"
  fi
  log "✅ Dependencies ready."
}

# ── Display Normalization ────────────────────────────────────────────────────
normalize_display() {
  local d="${DISPLAY:-}"
  # Fix DISPLAY=0/1 → :0/:1
  [[ "$d" =~ ^[0-9]+$ ]] && DISPLAY=":$d"
  # If no socket exists for current DISPLAY, clear it
  if [[ -n "$DISPLAY" ]]; then
    local num="${DISPLAY#:}"
    num="${num%%.*}"
    [[ -S "/tmp/.X11-unix/X${num}" ]] || unset DISPLAY
  fi
  # Fallback to first available socket
  if [[ -z "$DISPLAY" ]]; then
    local sock
    sock="$(find /tmp/.X11-unix/ -maxdepth 1 -name 'X*' -type s 2>/dev/null | head -1 || true)"
    [[ -n "$sock" ]] && DISPLAY=":${sock##*/X}"
  fi
  export DISPLAY
}

# ── Compact Diagnostics ──────────────────────────────────────────────────────
check_display() {
  echo "🔍 Display Diagnostics"
  [[ -z "$DISPLAY" ]] && { warn "DISPLAY is unset"; return 1; }
  echo "  ✅ DISPLAY=$DISPLAY"

  local num="${DISPLAY#:}"
  num="${num%%.*}"
  if [[ -S "/tmp/.X11-unix/X${num}" ]]; then
    echo "  ✅ X11 Socket: /tmp/.X11-unix/X${num}"
  else
    warn "Missing X11 socket for display $num"
    return 1
  fi

  if xdpyinfo >/dev/null 2>&1; then
    echo "  ✅ X Server: Connected"
  else
    warn "Cannot connect to X server"
    return 1
  fi

  local renderer
  renderer=$(glxinfo -B 2>/dev/null | grep "OpenGL renderer" | awk -F': ' '{print $2}' || echo "N/A")
  [[ -n "$renderer" ]] && echo "  🎮 GL Renderer: $renderer"
  return 0
}

# ── Launch ───────────────────────────────────────────────────────────────────
launch() {
  check_display || err "Display check failed. Use --debug to diagnose."
  echo "🚀 Launching: $APP"
  case "$APP" in
    xeyes|xclock|glxgears) exec "$APP" ;;
    *) exec bash -c "$APP" ;;
  esac
}

# ── Main ─────────────────────────────────────────────────────────────────────
main() {
  parse_args "$@"
  normalize_display

  if [[ "$MODE" == "check" ]]; then
    if check_display; then
      log "✅ Setup looks good."
      exit 0
    else
      err "❌ Display check failed."
    fi
  fi

  ensure_deps
  normalize_display  # Re-evaluate after potential env changes

  if [[ "$MODE" == "debug" ]]; then
    check_display
    echo
    echo "========== Environment =========="
    env | grep -E '^(DISPLAY|WAYLAND|XDG|XAUTHORITY|PULSE)' | sort || true
    echo "========== X11 Sockets =========="
    ls -la /tmp/.X11-unix/ 2>/dev/null || echo "  (none found)"
    echo "========== X Authority =========="
    xauth list 2>/dev/null || echo "  (none)"
    exit 0
  fi

  launch
}

main "$@"
