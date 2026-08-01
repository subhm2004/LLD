#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building tic_tac_toe_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "tic_tac_toe_app"
echo "Run: ./tic_tac_toe_app"
