#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building lru_cache_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "lru_cache_app"
echo "Run: ./lru_cache_app"
