#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building parking_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "parking_app"
echo "Run: ./parking_app"
