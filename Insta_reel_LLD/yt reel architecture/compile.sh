#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building reels_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "reels_app"
echo "Run: ./reels_app"
