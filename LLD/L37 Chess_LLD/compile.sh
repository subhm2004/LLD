#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building chess_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "chess_app"
echo "Run: ./chess_app"
