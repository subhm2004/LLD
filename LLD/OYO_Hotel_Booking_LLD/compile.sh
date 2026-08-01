#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building oyo_hotel_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "oyo_hotel_app"
echo "Run: ./oyo_hotel_app"
