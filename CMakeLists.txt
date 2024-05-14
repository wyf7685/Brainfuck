cmake_minimum_required(VERSION 3.5)

project(Brainfuck)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

aux_source_directory(src SRC_LIST)

add_executable(bf ${SRC_LIST})
target_include_directories(bf PUBLIC include)

add_executable(bfdbg ${SRC_LIST})
target_compile_definitions(bfdbg PRIVATE BF_DEBUG)
target_include_directories(bfdbg PUBLIC include)

add_custom_target(
    python_module ALL
    COMMAND python ${CMAKE_SOURCE_DIR}/setup.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building Python module"
)
