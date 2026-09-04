$ErrorActionPreference = "Stop"

$CEF_VERSION = "151.3.24%2Bg2384915%2Bchromium-151.0.7922.174_windows64"
$CEF_URL = "https://cef-builds.spotifycdn.com/cef_binary_$CEF_VERSION.tar.bz2"
$CEF_DIR = "$env:USERPROFILE\Downloads\cef_binary"
$CEF_ARCHIVE = "$env:USERPROFILE\Downloads\cef_binary.tar.bz2"

Write-Host "=== pybindcef Build Script for Windows ==="
winget install -e --id Microsoft.VisualStudio.BuildTools --override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended" --source winget
winget install -e --id Kitware.CMake --source winget

if (-not (Test-Path "$CEF_DIR\build\libcef_dll_wrapper\Release\libcef_dll_wrapper.lib")) {
    Write-Host "[*] Downloading and building CEF Wrapper..."
    if (-not (Test-Path $CEF_DIR)) {
        New-Item -ItemType Directory -Force -Path $CEF_DIR | Out-Null
    }
    
    if (-not (Test-Path $CEF_ARCHIVE)) {
        Write-Host "[*] Downloading CEF..."
        Invoke-WebRequest -Uri $CEF_URL -OutFile $CEF_ARCHIVE
    }
    
    Write-Host "[*] Extracting CEF..."
    tar -xjf $CEF_ARCHIVE -C $CEF_DIR --strip-components=1
    
    Write-Host "[*] Compiling libcef_dll_wrapper..."
    New-Item -ItemType Directory -Force -Path "$CEF_DIR\build" | Out-Null
    Push-Location "$CEF_DIR\build"
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    cmake --build . --config Release --parallel
    Pop-Location
} else {
    Write-Host "[*] CEF Wrapper already built."
}

Write-Host "[*] Building and installing pybindcef python package..."
pip install .

Write-Host "=== Build & Installation Complete ==="
Write-Host "You can now 'import pybindcef' in your Python scripts!"
