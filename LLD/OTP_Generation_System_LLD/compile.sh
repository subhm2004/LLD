#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building otp_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "otp_app"
echo "Run: ./otp_app"
