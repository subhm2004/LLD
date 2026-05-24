#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building ecommerce_checkout_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "ecommerce_checkout_app"
echo "Run: ./ecommerce_checkout_app"
