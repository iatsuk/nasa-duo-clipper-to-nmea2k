#!/usr/bin/env python3
"""Compile and run the exact production decoder, without an ESP32 or PlatformIO."""
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix="duet-test-") as temp:
    binary = str(Path(temp) / "test_core")
    subprocess.run([
        os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
        "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g",
        "-I" + str(root / "lib/duet_core/src"),
        str(root / "tests/test_core.cpp"), str(root / "lib/duet_core/src/duet.cpp"),
        "-o", binary,
    ], check=True)
    subprocess.run([binary], check=True)
