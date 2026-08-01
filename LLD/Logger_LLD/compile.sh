#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building logger_app (C++17)..."
"$CXX" "${FLAGS[@]}" "Main.cpp" -o "logger_app"
echo "Run: ./logger_app"
