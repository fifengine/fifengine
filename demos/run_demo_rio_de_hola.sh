#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/out/build/clang20-x64-linux-rel"
# Use the source python package directory so demos import the working copy
ENGINE_PYTHON_DIR="$REPO_ROOT/src/python"

cd "$REPO_ROOT"

echo "=== Step 1: Configure with CMake (clang20-x64-linux-rel) ==="
cmake --preset clang20-x64-linux-rel -DENABLE_FIFEGUI=ON -DBUILD_PYTHON_BINDINGS=ON -DBUILD_LIBRARY=ON

echo "=== Step 2: Build the project ==="
cmake --build "$BUILD_DIR" --target fife fife_swig

echo "=== Step 3: Setup Python bindings (use source package) ==="
cp -n "$BUILD_DIR/_fife_swig.so" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -n "$BUILD_DIR/fife.py" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -n "$BUILD_DIR/_fifechan_swig.so" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true
cp -n "$BUILD_DIR/fifechan.py" "$ENGINE_PYTHON_DIR/fife/" 2>/dev/null || true

echo "=== Step 4: Create Settings (if needed) ==="
cd "$SCRIPT_DIR/rio_de_hola"
if [ ! -f settings.xml ]; then
  cp settings-dist.xml settings.xml
fi

echo "=== Step 5: Run rio_de_hola demo ==="
export PYTHONPATH="$ENGINE_PYTHON_DIR:$PYTHONPATH"
export LD_LIBRARY_PATH="$BUILD_DIR:/workspaces/fifengine/out/fife-dependencies/install/lib:/workspaces/fifengine/vcpkg_installed/x64-linux/lib:$LD_LIBRARY_PATH"

python3 run.py
