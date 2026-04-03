# pybindcef
High-performance Python bindings for Chromium Embedded Framework (CEF)

## Build Instructions

### Building `libcef_dll_wrapper`

- Linux

    Download cef minimal build for your system architecture via https://cef-builds.spotifycdn.com

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

- Linux

    Install bare bones extension from pypi:
    ```bash
    pip install pybindcef
    ```

    OR BUILD FROM SOURCE:

    Clone this repository.

    ```bash
    git clone https://github.com/Novfensec/pybindcef -b main --single-branch --depth 1
    ```

    Now build the extension with `-DPython_EXECUTABLE="pythonexecutablewithversion"`

    ```bash
    mkdir pybindcef/build
    cd pybindcef/build

    cmake .. -DPython_EXECUTABLE=/usr/bin/python
    make
    ```

### Building `cef_worker`

- Linux

    ```bash
    mkdir pybindcef/cef_worker/build
    cd pybindcef/cef_worker/build

    cmake ..
    make
    ```

### Extracting resources

Copy all files under `cef_binary/Resources` to `cef_binary/Release`

```bash
cp -r ~/Downloads/cef_binary/Resources/* ~/Downloads/cef_binary/Release/
```

## Tests

### Linux

1. Kivy

    - Copy `pybindcef` extension and `cef_worker` executable to `pybindcef/tests/kivy/`.
    - Run main.py with the python executable version suitable for the extension.
    ```bash
    python main.py
    ```

2. PyQt

    - Copy `pybindcef` extension and `cef_worker` executable to `pybindcef/tests/pyqt/`.
    - Run main.py with the python executable version suitable for the extension.
    ```bash
    python main.py
    ```

3. Tkinter

    - Copy `pybindcef` extension and `cef_worker` executable to `pybindcef/tests/tkinter/`.
    - Run main.py with the python executable version suitable for the extension.
    ```bash
    python main.py
    ```


