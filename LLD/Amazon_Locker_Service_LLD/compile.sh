#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building amazon_locker_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o amazon_locker_app
echo "Run: ./amazon_locker_app"
