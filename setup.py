import platform
import sys

from setuptools import Extension, setup

if platform.system() == "Windows":
    cxx_std_arg = "/std:c++20"
else:
    cxx_std_arg = "-std=c++20"

extension = Extension(
    "brainfuck",
    sources=[
        "_bf.cpp",
        "src/interpreter.cpp",
        "src/instream.cpp",
        "src/loader.cpp",
        "src/utils.cpp",
    ],
    include_dirs=["include"],
    define_macros=[("_BRAINFUCK_RELEASE", None)],
    extra_compile_args=[cxx_std_arg],
)

sys.argv[1:] = ["build_ext", "--inplace"]
setup(
    name="brainfuck",
    version="1.0",
    description="BrainFuck interpreter by wyf7685",
    author="wyf7685",
    author_email="wyf7685@163.com",
    ext_modules=[extension],
)
