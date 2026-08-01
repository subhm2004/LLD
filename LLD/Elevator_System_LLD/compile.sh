#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building elevator_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "elevator_app"
echo "Run: ./elevator_app"
