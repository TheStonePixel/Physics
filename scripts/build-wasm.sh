#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build-wasm"
PKG_DIR="$ROOT_DIR/pkg"

# Check for emscripten
if ! command -v emcmake &> /dev/null; then
    echo "Error: emcmake not found. Install Emscripten and source emsdk_env.sh first."
    exit 1
fi

echo "Building WASM..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
emcmake cmake "$ROOT_DIR"
emmake make -j"$(nproc 2>/dev/null || echo 4)"

echo "Copying to pkg/..."
cp "$BUILD_DIR/physics_wasm.js" "$PKG_DIR/"
cp "$BUILD_DIR/physics_wasm.wasm" "$PKG_DIR/"

echo "Done. Package ready in pkg/"
