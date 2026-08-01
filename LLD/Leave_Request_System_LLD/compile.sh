#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building leave_request_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o leave_request_app
echo "Run: ./leave_request_app"
