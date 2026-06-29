#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building snake_ladder_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "snake_ladder_app"
echo "Run: ./snake_ladder_app"
