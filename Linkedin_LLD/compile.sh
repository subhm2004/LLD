#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building linkedin_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "linkedin_app"
echo "Run: ./linkedin_app"
