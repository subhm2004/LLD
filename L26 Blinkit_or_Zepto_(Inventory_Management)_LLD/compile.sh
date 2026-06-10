#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-g++}"
OUT="blinkit_zepto_app"

"$CXX" -std=c++17 -Wall -Wextra -pedantic main.cpp -o "$OUT"
echo "Built ./$OUT"
