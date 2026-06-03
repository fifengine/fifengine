#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/out/build/clang22-x64-linux-dbg-cov"
# Use the source python package directory so demos import the working copy
ENGINE_PYTHON_DIR="$REPO_ROOT/src/python"

export PYTHONPATH="$ENGINE_PYTHON_DIR:$PYTHONPATH"
export LD_LIBRARY_PATH="$BUILD_DIR:$REPO_ROOT/vcpkg_installed/x64-linux/lib:/usr/lib64:$REPO_ROOT/out/fife-dependencies/x64-linux/install/lib:$LD_LIBRARY_PATH"

cd "$REPO_ROOT"

echo "=== Step 1: Configure with CMake (clang22-x64-linux-dbg-cov) ==="
cmake --preset clang22-x64-linux-dbg-cov -DENABLE_FIFEGUI=ON -DBUILD_PYTHON_BINDINGS=ON -DBUILD_LIBRARY=ON

echo "=== Step 2: Build the project ==="
cmake --build "$BUILD_DIR" --target fife fife_swig fifechan_swig

echo "=== Step 3: Setup Python bindings (use source package) ==="
cp -f "$BUILD_DIR/_fife_swig.so" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -f "$BUILD_DIR/fife.py" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -f "$BUILD_DIR/_fifechan_swig.so" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -f "$BUILD_DIR/fifechan.py" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true

echo "=== Step 4: Create Settings (if needed) ==="
cd "$SCRIPT_DIR/pychan"
if [ ! -f settings.xml ]; then
  cp settings-dist.xml settings.xml
fi

echo "=== Step 5: Run pychan demo ==="

# Verify we're using the rebuilt library
echo "=== Verifying library loading ==="
ls -la "$BUILD_DIR/libfifengine.so.0.5.0" || echo "ERROR: libfifengine.so.0.5.0 not found!"

# Check ldd to see what's loaded
echo "=== Library dependencies ==="
ldd "$BUILD_DIR/_fife_swig.so" 2>/dev/null | grep fife || echo "No fife lib found"

python3 run.py
