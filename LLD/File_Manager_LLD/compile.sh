#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building file_manager_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "file_manager_app"
echo "Run: ./file_manager_app"
