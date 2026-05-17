#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building car_rental_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "car_rental_app"
echo "Run: ./car_rental_app"
