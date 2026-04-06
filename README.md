# pybindcef
High-performance Python bindings for Chromium Embedded Framework (CEF)

Thanks to [pybind11](https://github.com/pybind/pybind11) and [CEF](https://github.com/chromiumembedded/cef) for existing in this world.

## Build Instructions

### Building `libcef_dll_wrapper`

- Linux

    Download cef minimal build for your system architecture via https://cef-builds.spotifycdn.com

    ```bash
    mkdir -p ~/Downloads/cef_binary

    wget https://cef-builds.spotifycdn.com/cef_binary_146.0.9%2Bg3ca6a87%2Bchromium-146.0.7680.165_linux64_minimal.tar.bz2 -O ~/Downloads/cef_binary.tar.bz2

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
    ls ~/Downloads/cef_binary/build/libcef_dll_wrapper/
    ```

    A file named `libcef_dll_wrapper.a` will be listed.

- Windows

    Download cef minimal build for your system architecture via https://cef-builds.spotifycdn.com

    > **Powershell** commands below

    ```powershell
    mkdir $env:USERPROFILE\Downloads\cef_binary

    wget "https://cef-builds.spotifycdn.com/cef_binary_146.0.9%2Bg3ca6a87%2Bchromium-146.0.7680.165_windows64_minimal.tar.bz2" `
     -O $env:USERPROFILE\Downloads\cef_binary.tar.bz2

    tar -xjf $env:USERPROFILE\Downloads\cef_binary.tar.bz2 -C $env:USERPROFILE\Downloads\cef_binary --strip-components=1
    ```

    Now build the `libcef_dll_wrapper`

    ```powershell
    mkdir $env:USERPROFILE\Downloads\cef_binary\build
    cd $env:USERPROFILE\Downloads\cef_binary\build

    # enable msvc build environment for you system x64 x86_64 x86 amd64
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

    Install bare bones extension from pypi:
    ```bash
    pip install pybindcef
    ```

    OR BUILD FROM SOURCE:

    Install pybind11.

    ```bash
    pip install "pybind11[global]"
    ```

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

- Windows

    Install pybind11.

    ```bash
    pip install "pybind11[global]"
    ```

    Clone this repository.

    ```bash
    git clone https://github.com/Novfensec/pybindcef -b main --single-branch --depth 1
    ```

    Now build the extension.

    ```powershell
    mkdir pybindcef/build
    cd pybindcef/build

    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    ```

### Building `cef_worker`

- Linux

    ```bash
    mkdir pybindcef/cef_worker/build
    cd pybindcef/cef_worker/build

    cmake ..
    make
    ```

- Window

    ```powershell
    mkdir pybindcef/cef_worker/build
    cd pybindcef/cef_worker/build

    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    ```

### Extracting resources

- Linux

    Copy all files under `cef_binary/Resources` to `cef_binary/Release`

    ```bash
    cp -r ~/Downloads/cef_binary/Resources/* ~/Downloads/cef_binary/Release/
    ```

- Windows

    Copy all files under `cef_binary/Resources` and `cef_binary/Release` right next to the extension from wherever you are accessing it.
    All you need to do is make `libcef.dll` available in LD_LIBRARY_PATH and place the files under `cef_binary/Resources` next to `libcef.dll` that's it.

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

### Windows

1. Kivy

    - Copy `pybindcef` extension and `cef_worker` executable to `pybindcef/tests/kivy/` while also extracting resources as above instructions.
    - Run main.py with the python executable version suitable for the extension.
    ```bash
    python main.py
    ```

2. PyQt

    - Copy `pybindcef` extension and `cef_worker` executable to `pybindcef/tests/pyqt/` while also extracting resources as above instructions.
    - Run main.py with the python executable version suitable for the extension.
    ```bash
    python main.py
    ```

3. Tkinter

    - Copy `pybindcef` extension and `cef_worker` executable to `pybindcef/tests/tkinter/` while also extracting resources as above instructions.
    - Run main.py with the python executable version suitable for the extension.
    ```bash
    python main.py
    ```
