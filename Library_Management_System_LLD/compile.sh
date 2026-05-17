#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building library_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "library_app"
echo "Run: ./library_app"
