#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building vending_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "vending_app"
echo "Run: ./vending_app"
