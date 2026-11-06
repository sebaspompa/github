cmake_minimum_required(VERSION 3.20)

project(github-console
    VERSION 1.0.0
    LANGUAGES CXX
)

# ============================================================
# C++20
# ============================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# github-console
# ============================================================

add_executable(github-console
    src/main.cpp
)

target_compile_definitions(github-console PRIVATE
    PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

# Permite utilizar includes como:
#
# #include "src/app/App.hpp"
#
target_include_directories(github-console
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ============================================================
# Compiler warnings
# ============================================================

if(MSVC)

    target_compile_options(github-console PRIVATE
        /W4
    )

else()

    target_compile_options(github-console PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()