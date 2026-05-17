#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -Wpedantic -pthread)
OUT="${1:-lfu_cache_app}"

echo "Compiler: $("$CXX" --version | head -1)"
echo "Flags:    ${FLAGS[*]}"

"$CXX" "${FLAGS[@]}" main.cpp -o "$OUT"
echo "Built: $OUT"
./"$OUT"
