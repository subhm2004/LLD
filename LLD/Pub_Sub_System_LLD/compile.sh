#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building pubsub_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "pubsub_app"
echo "Run: ./pubsub_app"
