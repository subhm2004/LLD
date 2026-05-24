#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pedantic)
DIR="$(cd "$(dirname "$0")" && pwd)"
CODE="$DIR/C++ Code"
BIN="$DIR/bin"
mkdir -p "$BIN"

echo "Building L2 OOPS_1 (C++17)..."
for src in "$CODE"/*.cpp; do
  base="$(basename "$src" .cpp)"
  echo "  $base.cpp -> bin/$base"
  "$CXX" "${FLAGS[@]}" "$src" -o "$BIN/$base"
done
echo ""
echo "Done. Examples:"
echo "  $BIN/08_Encapsulation"
echo "  $BIN/04_Static_Members"
echo "  $BIN/06_Friend_Function"
