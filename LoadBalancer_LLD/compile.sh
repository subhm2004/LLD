#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building load_balancer_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "load_balancer_app"
echo "Run: ./load_balancer_app"
