#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building splitwise_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "splitwise_app"
echo "Run: ./splitwise_app"
