#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building atm_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "atm_app"
echo "Run: ./atm_app"
