#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/out/build/clang20-x64-linux-rel"
ENGINE_PYTHON_DIR="$REPO_ROOT/engine/python"
FIFE_DEPS_INSTALL_DIR="/out/fife-dependencies/install"

cd "$REPO_ROOT"

echo "=== Step 1: Check fifechan dependency ==="
if [ ! -d "$FIFE_DEPS_INSTALL_DIR" ]; then
    echo "Warning: fifechan not found at $FIFE_DEPS_INSTALL_DIR"
    echo "Please run /dependencies/setup.sh first to install fifechan and its dependencies."
    exit 1
fi

echo "=== Step 2: Configure with CMake (clang20-x64-linux-rel) ==="
cmake --preset clang20-x64-linux-rel -Dfifechan=ON -Dbuild-python=ON -Dbuild-library=ON

echo "=== Step 3: Build the project ==="
cmake --build "$BUILD_DIR" --target fife fife_swig fifechan_swig

echo "=== Step 4: Setup Python bindings ==="
mkdir -p "$ENGINE_PYTHON_DIR"

if [ ! -L "$ENGINE_PYTHON_DIR/fife" ] && [ ! -d "$ENGINE_PYTHON_DIR/fife" ]; then
    ln -sf "$REPO_ROOT/src/python/fife" "$ENGINE_PYTHON_DIR/fife"
    echo "Linked src/python/fife -> engine/python/fife"
fi

cp -f "$BUILD_DIR/_fife_swig.so" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -f "$BUILD_DIR/fife.py" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -f "$BUILD_DIR/_fifechan_swig.so" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -f "$BUILD_DIR/fifechan.py" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true

echo "=== Step 5: Copy settings file if needed ==="
cd "$SCRIPT_DIR/rpg"
if [ ! -f settings.xml ]; then
    cp settings-dist.xml settings.xml
fi

echo "=== Step 6: Install Python dependencies ==="
pip3 install future --break-system-packages 2>/dev/null || true

echo "=== Step 7: Run rpg demo ==="
export PYTHONPATH="$ENGINE_PYTHON_DIR:$PYTHONPATH"
export LD_LIBRARY_PATH="$BUILD_DIR:$FIFE_DEPS_INSTALL_DIR/lib:$REPO_ROOT/vcpkg_installed/x64-linux/lib:$LD_LIBRARY_PATH"

python3 run.py
