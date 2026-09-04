#!/bin/bash
set -e

CEF_VERSION="151.3.24%2Bg2384915%2Bchromium-151.0.7922.174_linux64"
CEF_URL="https://cef-builds.spotifycdn.com/cef_binary_${CEF_VERSION}.tar.bz2"
CEF_DIR="$HOME/Downloads/cef_binary"
CEF_ARCHIVE="$HOME/Downloads/cef_binary.tar.bz2"

echo "=== pybindcef Build Script for Linux ==="
sudo apt update
sudo apt install -y build-essential cmake ninja-build

if [ ! -f "$CEF_DIR/build/libcef_dll_wrapper/libcef_dll_wrapper.a" ]; then
    echo "[*] Downloading and building CEF Wrapper..."
    mkdir -p "$CEF_DIR"
    
    if [ ! -f "$CEF_ARCHIVE" ]; then
        wget -O "$CEF_ARCHIVE" "$CEF_URL"
    fi
    
    echo "[*] Extracting CEF..."
    tar -xjf "$CEF_ARCHIVE" -C "$CEF_DIR" --strip-components=1
    
    echo "[*] Compiling libcef_dll_wrapper..."
    mkdir -p "$CEF_DIR/build"
    cd "$CEF_DIR/build"
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    make -j$(nproc)
    cd -
else
    echo "[*] CEF Wrapper already built."
fi

echo "[*] Building and installing pybindcef python package..."
pip install .

echo "=== Build & Installation Complete ==="
echo "You can now 'import pybindcef' in your Python scripts!"
