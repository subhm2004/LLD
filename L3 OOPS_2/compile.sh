#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pedantic)
DIR="$(cd "$(dirname "$0")" && pwd)"
CODE="$DIR/C++ Code"
BIN="$DIR/bin"
mkdir -p "$BIN"

echo "Building L3 OOPS_2 (C++17)..."
for src in "$CODE"/*.cpp; do
  base="$(basename "$src" .cpp)"
  echo "  $base.cpp -> bin/$base"
  "$CXX" "${FLAGS[@]}" "$src" -o "$BIN/$base"
done
echo "Run: $BIN/Inheritance  |  $BIN/DynamicPolymorphism"
