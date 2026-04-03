# pybindcef
Python bindings for CEF (chromium embedded framework).

## Build Instructions

### Building `libcef_dll_wrapper`

Download cef minimal build for your system architecture via https://cef-builds.spotifycdn.com

- Linux

    ```bash
    mkdir -p ~/Downloads/cef_binary

    wget https://cef-builds.spotifycdn.com/cef_binary_146.0.9%2Bg3ca6a87%2Bchromium-146.0.7680.165_linux64_minimal.tar.bz2 -O ~/Downloads/cef_binary.tar.bz2

    tar -xjf ~/Downloads/cef_binary.tar.bz2 -C ~/Downloads/cef_binary --strip-components=1
    ```

    Now build the libcef_dll_wrapper

    ```bash
    mkdir ~/Downloads/cef_binary/build
    cd ~/Downloads/cef_binary/build

    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    make -j$(nproc)
    ```

- Confirm the build

    ```
    ls ~/Downloads/cef_binary/build/libcef_dll_wrapper/
    ```

    A file named `libcef_dll_wrapper.a` will be listed.

### Building `pybindcef` extension

Clone this repository.

- Linux

    ```bash
    git clone https://github.com/Novfensec/pybindcef
    ```

    Now build the extension; `-DPython_EXECUTABLE="pythonexecutablewithversion"`

    ```bash
    mkdir build
    cd build

    cmake .. -DPython_EXECUTABLE=/usr/bin/python
    make
    ```

    Now build `cef_worker`

    ```bash
    mkdir cef_worker/build
    cd cef_worker/build

    cmake ..
    make
    ```

