#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building discount_coupon_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "discount_coupon_app"
echo "Run: ./discount_coupon_app"
