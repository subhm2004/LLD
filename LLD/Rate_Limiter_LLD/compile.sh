#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building rate_limiter_app (C++17)..."
"$CXX" "${FLAGS[@]}" "Main.cpp" -o "rate_limiter_app"
echo "Run: ./rate_limiter_app"
