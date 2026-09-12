import os
import platform
import sys

PACKAGE_DIR = os.path.dirname(os.path.abspath(__file__))

if os.name == "nt":
    SHARE_DIR = os.path.join(sys.prefix, "share", "pybindcef")
    if hasattr(os, "add_dll_directory"):
        os.add_dll_directory(SHARE_DIR)
    os.environ["PATH"] = f"{SHARE_DIR};{PACKAGE_DIR};{os.environ.get('PATH', '')}"
else:
    SHARE_DIR = os.path.join(os.path.dirname(PACKAGE_DIR), "pybindcef.libs")
    os.environ["LD_LIBRARY_PATH"] = f"{SHARE_DIR}:{os.environ.get('LD_LIBRARY_PATH', '')}"

from ._pybindcef import * # type: ignore

if platform.system() == "Windows":
    WORKER_EXE = os.path.join(SHARE_DIR, "cef_worker.exe")
else:
    WORKER_EXE = os.path.join(SHARE_DIR, "cef_worker")

RESOURCES_DIR = SHARE_DIR
