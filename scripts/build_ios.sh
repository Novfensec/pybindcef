#!/usr/bin/env bash
#
# build_ios.sh - Build CEF (Chromium Embedded Framework) for iOS
#
# This script automates the process of:
#   1. Fetching/checking out the CEF source (via depot_tools + chromium)
#   2. Configuring a GN build for iOS (arm64)
#   3. Building the CEF framework and libcef_dll_wrapper for iOS
#
# Prerequisites:
#   - macOS with Xcode and Command Line Tools installed
#   - At least 100 GB of free disk space
#   - ~16 GB RAM recommended
#   - Python 3
#   - Git
#
# Usage:
#   ./scripts/build_ios.sh [OPTIONS]
#
# Options:
#   --cef-branch <branch>     CEF branch to build (default: 6834 i.e. Chromium 134)
#   --build-dir <path>        Build directory (default: ~/cef_ios_build)
#   --jobs <n>                Parallel jobs for compilation (default: $(sysctl -n hw.ncpu))
#   --target-sdk <sdk>        iOS SDK version (default: latest)
#   --min-ios-version <ver>   Minimum iOS deployment target (default: 15.0)
#   --skip-fetch              Skip fetching/syncing source (use existing checkout)
#   --release-only            Only build Release configuration
#   --simulator               Build for iOS Simulator (x86_64/arm64) instead of device
#   --help                    Show this help message
#
# CEF iOS Build Notes:
#   - CEF does NOT provide prebuilt binaries for iOS on cef-builds.spotifycdn.com
#   - iOS builds must be compiled from source using the Chromium build system
#   - The iOS build produces a framework (.framework) instead of .dylib/.so
#   - WKWebView is the underlying engine on iOS (Apple requirement)
#   - CEF on iOS wraps WKWebView with the CEF API surface
#
set -euo pipefail

# ============================================================================
# Configuration defaults
# ============================================================================
CEF_BRANCH="6834"
BUILD_DIR="${HOME}/cef_ios_build"
JOBS=$(sysctl -n hw.ncpu 2>/dev/null || echo 8)
TARGET_SDK=""
MIN_IOS_VERSION="15.0"
SKIP_FETCH=false
RELEASE_ONLY=false
BUILD_SIMULATOR=false

# ============================================================================
# Color output helpers
# ============================================================================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info()  { echo -e "${BLUE}[INFO]${NC} $*"; }
log_ok()    { echo -e "${GREEN}[OK]${NC} $*"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
log_error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }

# ============================================================================
# Parse arguments
# ============================================================================
while [[ $# -gt 0 ]]; do
    case "$1" in
        --cef-branch)
            CEF_BRANCH="$2"; shift 2 ;;
        --build-dir)
            BUILD_DIR="$2"; shift 2 ;;
        --jobs)
            JOBS="$2"; shift 2 ;;
        --target-sdk)
            TARGET_SDK="$2"; shift 2 ;;
        --min-ios-version)
            MIN_IOS_VERSION="$2"; shift 2 ;;
        --skip-fetch)
            SKIP_FETCH=true; shift ;;
        --release-only)
            RELEASE_ONLY=true; shift ;;
        --simulator)
            BUILD_SIMULATOR=true; shift ;;
        --help)
            head -n 40 "$0" | grep '^#' | sed 's/^# \?//'
            exit 0 ;;
        *)
            log_error "Unknown option: $1"
            exit 1 ;;
    esac
done

# ============================================================================
# Preflight checks
# ============================================================================
log_info "=== CEF iOS Build Script ==="
log_info "CEF Branch: ${CEF_BRANCH}"
log_info "Build Directory: ${BUILD_DIR}"
log_info "Parallel Jobs: ${JOBS}"
log_info "Min iOS Version: ${MIN_IOS_VERSION}"
log_info "Build Simulator: ${BUILD_SIMULATOR}"
echo ""

# Check macOS
if [[ "$(uname)" != "Darwin" ]]; then
    log_error "This script must be run on macOS (required for iOS builds)."
    exit 1
fi

# Check Xcode
if ! xcode-select -p &>/dev/null; then
    log_error "Xcode Command Line Tools not found. Install with: xcode-select --install"
    exit 1
fi

XCODE_PATH=$(xcode-select -p)
log_ok "Xcode found at: ${XCODE_PATH}"

# Check available disk space (need ~100GB)
AVAILABLE_GB=$(df -g "${BUILD_DIR%/*}" 2>/dev/null | awk 'NR==2 {print $4}' || echo "0")
if [[ "${AVAILABLE_GB}" -lt 80 ]]; then
    log_warn "Low disk space: ${AVAILABLE_GB} GB available. Chromium source + build needs ~100 GB."
    log_warn "Proceeding anyway, but build may fail if space runs out."
fi

# Check Python 3
if ! command -v python3 &>/dev/null; then
    log_error "Python 3 is required but not found."
    exit 1
fi

# Check git
if ! command -v git &>/dev/null; then
    log_error "Git is required but not found."
    exit 1
fi

# ============================================================================
# Step 1: Set up depot_tools
# ============================================================================
DEPOT_TOOLS_DIR="${BUILD_DIR}/depot_tools"

setup_depot_tools() {
    log_info "Setting up depot_tools..."
    
    if [[ -d "${DEPOT_TOOLS_DIR}" ]]; then
        log_info "depot_tools already exists, updating..."
        cd "${DEPOT_TOOLS_DIR}"
        git pull --quiet
    else
        mkdir -p "${BUILD_DIR}"
        git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git "${DEPOT_TOOLS_DIR}"
    fi
    
    export PATH="${DEPOT_TOOLS_DIR}:${PATH}"
    log_ok "depot_tools ready."
}

# ============================================================================
# Step 2: Fetch CEF source
# ============================================================================
CEF_SOURCE_DIR="${BUILD_DIR}/chromium_git"

fetch_cef_source() {
    log_info "Fetching CEF source (this will take a long time on first run)..."
    
    mkdir -p "${CEF_SOURCE_DIR}"
    cd "${CEF_SOURCE_DIR}"
    
    # Download automate-git.py from CEF
    if [[ ! -f "automate-git.py" ]]; then
        log_info "Downloading CEF automate-git.py..."
        curl -sL "https://raw.githubusercontent.com/chromiumembedded/cef/master/tools/automate/automate-git.py" \
            -o automate-git.py
    fi
    
    # Create the update script
    cat > "${BUILD_DIR}/update.sh" << 'INNER_EOF'
#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/chromium_git"

python3 automate-git.py \
    --download-dir="$(pwd)" \
    --depot-tools-dir="DEPOT_TOOLS_PLACEHOLDER" \
    --branch="BRANCH_PLACEHOLDER" \
    --no-build \
    --no-distrib \
    --force-clean
INNER_EOF
    
    sed -i '' "s|DEPOT_TOOLS_PLACEHOLDER|${DEPOT_TOOLS_DIR}|g" "${BUILD_DIR}/update.sh"
    sed -i '' "s|BRANCH_PLACEHOLDER|${CEF_BRANCH}|g" "${BUILD_DIR}/update.sh"
    chmod +x "${BUILD_DIR}/update.sh"
    
    log_info "Running automate-git.py to fetch/sync sources..."
    log_warn "This may take several hours on first run (downloading ~30GB of source)."
    
    python3 automate-git.py \
        --download-dir="${CEF_SOURCE_DIR}" \
        --depot-tools-dir="${DEPOT_TOOLS_DIR}" \
        --branch="${CEF_BRANCH}" \
        --no-build \
        --no-distrib \
        --force-clean
    
    log_ok "CEF source fetched successfully."
}

# ============================================================================
# Step 3: Configure GN for iOS
# ============================================================================
CHROMIUM_SRC="${CEF_SOURCE_DIR}/chromium/src"

configure_gn_ios() {
    log_info "Configuring GN build for iOS..."
    
    cd "${CHROMIUM_SRC}"
    
    # Determine target CPU
    local target_cpu="arm64"
    local target_environment="device"
    
    if [[ "${BUILD_SIMULATOR}" == true ]]; then
        # For simulator, build for the host architecture
        if [[ "$(uname -m)" == "arm64" ]]; then
            target_cpu="arm64"
        else
            target_cpu="x64"
        fi
        target_environment="simulator"
    fi
    
    # iOS SDK path
    local ios_sdk_path
    if [[ "${BUILD_SIMULATOR}" == true ]]; then
        ios_sdk_path=$(xcrun --sdk iphonesimulator --show-sdk-path)
    else
        ios_sdk_path=$(xcrun --sdk iphoneos --show-sdk-path)
    fi
    
    local out_dir="out/Release_GN_ios_${target_cpu}"
    if [[ "${BUILD_SIMULATOR}" == true ]]; then
        out_dir="out/Release_GN_ios_sim_${target_cpu}"
    fi
    
    # GN args for iOS CEF build
    local gn_args=(
        "target_os=\"ios\""
        "target_cpu=\"${target_cpu}\""
        "target_environment=\"${target_environment}\""
        "is_debug=false"
        "is_component_build=false"
        "ios_deployment_target=\"${MIN_IOS_VERSION}\""
        "ios_enable_code_signing=false"
        "enable_dsyms=true"
        "enable_stripping=true"
        "use_cef=true"
        "is_official_build=true"
        "proprietary_codecs=true"
        "ffmpeg_branding=\"Chrome\""
        "treat_warnings_as_errors=false"
        # iOS-specific CEF settings
        "use_allocator=\"none\""
        "use_partition_alloc=false"
        "enable_remoting=false"
        "enable_nacl=false"
        "blink_symbol_level=0"
        "v8_symbol_level=0"
    )
    
    if [[ -n "${TARGET_SDK}" ]]; then
        gn_args+=("ios_sdk_version=\"${TARGET_SDK}\"")
    fi
    
    # Write args to file
    mkdir -p "${out_dir}"
    printf '%s\n' "${gn_args[@]}" > "${out_dir}/args.gn"
    
    log_info "GN args written to ${out_dir}/args.gn:"
    cat "${out_dir}/args.gn"
    echo ""
    
    # Run GN gen
    log_info "Running gn gen..."
    gn gen "${out_dir}"
    
    log_ok "GN configuration complete."
    echo "${out_dir}"
}

# ============================================================================
# Step 4: Build CEF for iOS
# ============================================================================
build_cef_ios() {
    local out_dir="$1"
    
    log_info "Building CEF for iOS (this will take a while)..."
    log_info "Output directory: ${out_dir}"
    log_info "Using ${JOBS} parallel jobs."
    
    cd "${CHROMIUM_SRC}"
    
    # Build the CEF framework and wrapper
    ninja -C "${out_dir}" -j "${JOBS}" cef_framework cef_sandbox libcef_dll_wrapper
    
    log_ok "CEF iOS build completed successfully!"
}

# ============================================================================
# Step 5: Build libcef_dll_wrapper static library for iOS
# ============================================================================
build_wrapper_standalone() {
    local out_dir="$1"
    
    log_info "Building libcef_dll_wrapper for iOS (standalone)..."
    
    cd "${CHROMIUM_SRC}"
    
    ninja -C "${out_dir}" -j "${JOBS}" libcef_dll_wrapper
    
    log_ok "libcef_dll_wrapper built successfully."
}

# ============================================================================
# Step 6: Package the output
# ============================================================================
package_output() {
    local out_dir="$1"
    local dist_dir="${BUILD_DIR}/dist/ios"
    
    log_info "Packaging build output..."
    
    mkdir -p "${dist_dir}/framework"
    mkdir -p "${dist_dir}/lib"
    mkdir -p "${dist_dir}/include"
    
    cd "${CHROMIUM_SRC}"
    
    # Copy framework
    if [[ -d "${out_dir}/CefFramework.framework" ]]; then
        cp -R "${out_dir}/CefFramework.framework" "${dist_dir}/framework/"
        log_ok "Copied CefFramework.framework"
    elif [[ -d "${out_dir}/Chromium Embedded Framework.framework" ]]; then
        cp -R "${out_dir}/Chromium Embedded Framework.framework" "${dist_dir}/framework/"
        log_ok "Copied Chromium Embedded Framework.framework"
    fi
    
    # Copy libcef_dll_wrapper
    if [[ -f "${out_dir}/obj/cef/libcef_dll_wrapper.a" ]]; then
        cp "${out_dir}/obj/cef/libcef_dll_wrapper.a" "${dist_dir}/lib/"
        log_ok "Copied libcef_dll_wrapper.a"
    elif [[ -f "${out_dir}/obj/libcef_dll_wrapper.a" ]]; then
        cp "${out_dir}/obj/libcef_dll_wrapper.a" "${dist_dir}/lib/"
        log_ok "Copied libcef_dll_wrapper.a"
    fi
    
    # Copy CEF headers
    local cef_dir="${CEF_SOURCE_DIR}/chromium/src/cef"
    if [[ -d "${cef_dir}/include" ]]; then
        cp -R "${cef_dir}/include" "${dist_dir}/"
        log_ok "Copied CEF headers"
    fi
    
    # Generate an iOS-specific CMake toolchain file
    cat > "${dist_dir}/ios-toolchain.cmake" << 'CMAKE_EOF'
# iOS CMake Toolchain for CEF
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=ios-toolchain.cmake ..

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "Minimum iOS version")
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)

# Find iOS SDK
execute_process(
    COMMAND xcrun --sdk iphoneos --show-sdk-path
    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
CMAKE_EOF
    
    log_ok "Generated ios-toolchain.cmake"
    
    # Create a summary
    cat > "${dist_dir}/BUILD_INFO.txt" << EOF
CEF iOS Build Information
=========================
CEF Branch: ${CEF_BRANCH}
Build Date: $(date -u +"%Y-%m-%d %H:%M:%S UTC")
Min iOS Version: ${MIN_IOS_VERSION}
Build Type: $(if [[ "${BUILD_SIMULATOR}" == true ]]; then echo "Simulator"; else echo "Device (arm64)"; fi)
Host: $(uname -a)
Xcode: $(xcodebuild -version | head -1)

Output Directory Structure:
  framework/   - CEF.framework (embed in your iOS app)
  lib/         - libcef_dll_wrapper.a (link statically)
  include/     - CEF C/C++ headers
  ios-toolchain.cmake - CMake toolchain file for cross-compilation

Usage with pybindcef:
  export CEF_ROOT=${dist_dir}
  export CEF_DLL_WRAPPER_ROOT=${dist_dir}/lib
  cmake -DCMAKE_TOOLCHAIN_FILE=${dist_dir}/ios-toolchain.cmake ..
EOF
    
    log_ok "Build output packaged to: ${dist_dir}"
    log_info ""
    log_info "=== Build Summary ==="
    log_info "Distribution: ${dist_dir}"
    ls -la "${dist_dir}/"
    echo ""
    log_info "To use with pybindcef:"
    log_info "  export CEF_ROOT=${dist_dir}"
    log_info "  export CEF_DLL_WRAPPER_ROOT=${dist_dir}/lib"
}

# ============================================================================
# Step 7 (Optional): Create XCFramework for universal distribution
# ============================================================================
create_xcframework() {
    local device_dir="${BUILD_DIR}/dist/ios/framework"
    local xcframework_dir="${BUILD_DIR}/dist/CEF.xcframework"
    
    if [[ -d "${device_dir}" ]]; then
        log_info "Creating XCFramework..."
        
        local framework_name
        if [[ -d "${device_dir}/CefFramework.framework" ]]; then
            framework_name="CefFramework.framework"
        elif [[ -d "${device_dir}/Chromium Embedded Framework.framework" ]]; then
            framework_name="Chromium Embedded Framework.framework"
        else
            log_warn "No framework found to create XCFramework."
            return 0
        fi
        
        xcodebuild -create-xcframework \
            -framework "${device_dir}/${framework_name}" \
            -output "${xcframework_dir}" 2>/dev/null || {
            log_warn "XCFramework creation failed (may need both device and simulator builds)."
            log_info "Run with --simulator flag separately, then combine manually."
        }
    fi
}

# ============================================================================
# Main execution
# ============================================================================
main() {
    local start_time=$(date +%s)
    
    log_info "Starting CEF iOS build process..."
    echo ""
    
    # Step 1: depot_tools
    setup_depot_tools
    echo ""
    
    # Step 2: Fetch source (unless skipped)
    if [[ "${SKIP_FETCH}" == false ]]; then
        fetch_cef_source
    else
        log_info "Skipping source fetch (--skip-fetch)."
        if [[ ! -d "${CHROMIUM_SRC}" ]]; then
            log_error "Source directory not found: ${CHROMIUM_SRC}"
            log_error "Run without --skip-fetch first."
            exit 1
        fi
    fi
    echo ""
    
    # Step 3: Configure GN
    local out_dir
    out_dir=$(configure_gn_ios)
    echo ""
    
    # Step 4: Build
    build_cef_ios "${out_dir}"
    echo ""
    
    # Step 5: Package
    package_output "${out_dir}"
    echo ""
    
    # Step 6: Create XCFramework (optional)
    create_xcframework
    echo ""
    
    local end_time=$(date +%s)
    local elapsed=$(( end_time - start_time ))
    local hours=$(( elapsed / 3600 ))
    local minutes=$(( (elapsed % 3600) / 60 ))
    
    log_ok "=== CEF iOS Build Complete ==="
    log_ok "Total time: ${hours}h ${minutes}m"
    log_ok "Output: ${BUILD_DIR}/dist/ios"
}

main "$@"
