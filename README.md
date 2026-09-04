# pybindcef
High-performance Python bindings for Chromium Embedded Framework (CEF)

Video testing: https://youtu.be/yWIah-r6sbw?si=O_uNuDDE8uSf6-Nk

> [!NOTE]
> Complete examples of using with Kivy, Tkinter and PyQt6 are under `examples`.

Thanks to [pybind11](https://github.com/pybind/pybind11) and [CEF](https://github.com/chromiumembedded/cef) for existing in this world.

Inspired from [cefpython](https://github.com/cztomczak/cefpython)

<!-- GitAds-Verify: N1APKAO73QIHMPQPGMQSCVEETCXBNDA7 -->

## GitAds Sponsored
[![Sponsored by GitAds](https://gitads.dev/v1/ad-serve?source=novfensec/pybindcef@github)](https://gitads.dev/v1/ad-track?source=novfensec/pybindcef@github)


## Financial Support
[![GitHub Sponsors](https://img.shields.io/github/sponsors/Novfensec?style=for-the-badge&label=Sponsor%20Novfensec&logo=github&color=000000)](https://github.com/sponsors/Novfensec)

[![Donate via](https://img.shields.io/badge/Donate%20via-Wise-9FE870?style=for-the-badge&logo=wise&labelColor=163300)](https://wise.com/pay/business/kartavyashukla)

[![Donate via PayPal](https://img.shields.io/badge/Donate%20via-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.me/KARTAVYASHUKLA)


## Automated Build Instructions
> [!NOTE]
> Only for versions > `0.1.0`

Build and Install using automated build scripts.

- Linux:
    ```sh
    chmod +x ./build.sh
    ./build.sh
    ```

- Windows (Powershell):
    ```powershell
    powershell -ExecutionPolicy ByPass -c .\build.ps1
    ```

## Manual Build Instructions
Better watch a video: 
- for `v0.1.0`: https://youtu.be/3ZYGRoq0yno?si=SHUavAi3QQssk8rD
- for `latest`: Not yet out there

<details>
  <summary>Latest build instructions</summary>

### Building `libcef_dll_wrapper`

- Linux

    Install necessary build tools:
    ```
    sudo apt update
    sudo apt install -y build-essential cmake ninja-build
    ```

    Download cef minimal build for your system architecture via https://cef-builds.spotifycdn.com/index.html

    ```bash
    mkdir -p ~/Downloads/cef_binary

    wget https://cef-builds.spotifycdn.com/cef_binary_151.3.24%2Bg2384915%2Bchromium-151.0.7922.174_linux64.tar.bz2 -O ~/Downloads/cef_binary.tar.bz2

    tar -xjf ~/Downloads/cef_binary.tar.bz2 -C ~/Downloads/cef_binary --strip-components=1
    ```

    Now build the `libcef_dll_wrapper`

    ```bash
    mkdir ~/Downloads/cef_binary/build
    cd ~/Downloads/cef_binary/build

    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    make -j$(nproc)
    ```

    Confirm the build

    ```
    ls ~/Downloads/cef_binary/build/libcef_dll_wrapper/Release
    ```

    A file named `libcef_dll_wrapper.a` will be listed.

- Windows

    Install the necessary C++ build tools and CMake using Windows Package Manager (`winget`):
    
    ```powershell
    # Install Visual C++ Build Tools
    winget install Microsoft.VisualStudio.BuildTools
    
    # Install CMake
    winget install Kitware.CMake
    ```

    Download cef minimal/standard build for your system architecture via https://cef-builds.spotifycdn.com/index.html

    > **Powershell** commands below

    ```powershell
    mkdir $env:USERPROFILE\Downloads\cef_binary

    wget "https://cef-builds.spotifycdn.com/cef_binary_151.3.24%2Bg2384915%2Bchromium-151.0.7922.174_windows64.tar.bz2" `
     -O $env:USERPROFILE\Downloads\cef_binary.tar.bz2

    tar -xjf $env:USERPROFILE\Downloads\cef_binary.tar.bz2 -C $env:USERPROFILE\Downloads\cef_binary --strip-components=1
    ```

    Now build the `libcef_dll_wrapper`

    ```powershell
    mkdir $env:USERPROFILE\Downloads\cef_binary\build
    cd $env:USERPROFILE\Downloads\cef_binary\build

    # enable msvc build environment for you system x64 x86_64 x86 amd64
    # You can start the developer command prompt and type `where vcvarsall` to see the exact path
    # then simply paste that path to run e.g. C:\Program Files (x86)\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat
    vcvarsall.bat amd64

    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    cmake --build . --config Release --parallel
    ```

    Confirm the build

    ```
    Get-ChildItem $env:USERPROFILE\Downloads\cef_binary\build\libcef_dll_wrapper\Release
    ```

    A file named `libcef_dll_wrapper.lib` will be listed.

### Building `pybindcef` extension

PyPI: https://pypi.org/project/pybindcef/

- Linux

    Install pybind11.

    ```bash
    pip install "pybind11[global]"
    ```

    Clone this repository.

    ```bash
    git clone https://github.com/Novfensec/pybindcef -b main --single-branch --depth 1
    ```

    Now install via pip:
    ```bash
    pip install .
    ```

- Windows

    Install pybind11.

    ```bash
    pip install "pybind11[global]"
    ```

    Clone this repository.

    ```bash
    git clone https://github.com/Novfensec/pybindcef -b main --single-branch --depth 1
    ```

    Now install via pip:
    ```powershell
    pip install .
    ```

</details>

## Running Examples

After building and installing via `pip install .` (or the automated scripts), you can run the examples directly:
```bash
python examples/kivy/main.py
```
The python module resolves the bundled CEF dependencies internally.

