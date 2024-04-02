import platform
from setuptools import setup, Extension

if platform.system() == "Windows":
    cxx_std_arg = "/std:c++20"
else:
    cxx_std_arg = "-std=c++20"

extension = Extension(
    "_brainfuck",
    sources=[
        "_bf.cpp",
        "src/interpreter.cpp",
        "src/instream.cpp",
        "src/loader.cpp",
        "src/utils.cpp",
    ],
    define_macros=[("_BRAINFUCK_RELEASE", None)],
    extra_compile_args=[cxx_std_arg],
)

setup(
    name="brainfuck",
    version="1.0",
    description="BrainFuck interpreter by wyf7685",
    author="wyf7685",
    author_email="wyf7685@163.com",
    ext_modules=[extension],
)
