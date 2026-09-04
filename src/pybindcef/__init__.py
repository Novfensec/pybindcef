import os
import platform

PACKAGE_DIR = os.path.dirname(os.path.abspath(__file__))

if os.name == "nt":
    if hasattr(os, "add_dll_directory"):
        os.add_dll_directory(PACKAGE_DIR)
    os.environ["PATH"] = f"{PACKAGE_DIR};{os.environ.get('PATH', '')}"

from ._pybindcef import * # type: ignore

if platform.system() == "Windows":
    WORKER_EXE = os.path.join(PACKAGE_DIR, "cef_worker.exe")
else:
    WORKER_EXE = os.path.join(PACKAGE_DIR, "cef_worker")

RESOURCES_DIR = PACKAGE_DIR
