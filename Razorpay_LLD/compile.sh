#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building razorpay_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o razorpay_app
echo "Run: ./razorpay_app"
