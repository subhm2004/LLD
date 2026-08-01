#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building leetcode_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "leetcode_app"
echo "Run: ./leetcode_app"
