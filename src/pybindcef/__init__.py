import os
import platform
import sys

PACKAGE_DIR = os.path.dirname(os.path.abspath(__file__))
SHARE_DIR = os.path.join(sys.prefix, "share", "pybindcef")

if os.name == "nt":
    if hasattr(os, "add_dll_directory"):
        os.add_dll_directory(SHARE_DIR)
    os.environ["PATH"] = f"{SHARE_DIR};{PACKAGE_DIR};{os.environ.get('PATH', '')}"

from ._pybindcef import * # type: ignore

if platform.system() == "Windows":
    WORKER_EXE = os.path.join(SHARE_DIR, "cef_worker.exe")
else:
    WORKER_EXE = os.path.join(SHARE_DIR, "cef_worker")

RESOURCES_DIR = SHARE_DIR
