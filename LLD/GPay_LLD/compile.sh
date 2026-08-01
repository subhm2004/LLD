#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building gpay_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o gpay_app
echo "Run: ./gpay_app"
