#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building airline_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "airline_app"
echo "Run: ./airline_app"
