#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building ride_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "ride_app"
echo "Run: ./ride_app"
