#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building cache_ttl_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o cache_ttl_app
echo "Run: ./cache_ttl_app"
