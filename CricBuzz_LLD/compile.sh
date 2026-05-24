#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building cricbuzz_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o cricbuzz_app
echo "Run: ./cricbuzz_app"
