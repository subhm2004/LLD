#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building tinder_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "tinder_app"
echo "Run: ./tinder_app"
