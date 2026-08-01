#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building consistent_hashing_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "consistent_hashing_app"
echo "Run: ./consistent_hashing_app"
